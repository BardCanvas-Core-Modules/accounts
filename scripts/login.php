<?php
/**
 * User login
 *
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * @param string "user_name"
 * @param string "password"
 * @param string "tfa_token" required when account has 2fa enabled
 * @param string "redir_url" optional, direct redirect after open session
 * @param string "goto"      optional, return a location to redirect to as part of the response
 *                           (handled by the login JS snippets)
 * 
 * @returns string
 * 
 * Returning strings:
 * • ERROR_MISSING_PARAMS
 * • ERROR_ACCOUNT_UNEXISTENT
 * • ERROR_ACCOUNT_DISABLED
 * • ERROR_WRONG_PASSWORD
 * • ERROR_ENGINE_DISABLED
 * • ERROR_DEVICE_DISABLED
 * • ERROR_MISSING_2FA_TOKEN
 * • ERROR_INVALID_2FA_TOKEN
 * • ERROR_2FA_VALIDATION_FAILED
 * • OK → User name → OK|UNREGISTERED → url
 *   Tab-separated fields: result, user name, device message, admin dashboard url or redir_url
 */

use hng2_base\account;
use hng2_base\device;

include "../../config.php";
include "../../includes/bootstrap.inc";

header("Content-Type: text/plain; charset=utf-8");

#region Bruteforce check
#=======================

$chk_ip  = get_remote_address();
$mem_key = "@accounts:attempts_count_by_ip:$chk_ip";
$mem_ttl = 60 * 10;
$blk_ttl = 60 * 60;
$bftries = 30;
$bf_hits = (int) $mem_cache->get($mem_key);

if( $bf_hits == $bftries )
{
    $logdate  = date("Ymd");
    $logfile  = "{$config->logfiles_location}/bruteforce_attempts-$logdate.log";
    $lognowd  = date("H:i:s");
    $location = forge_geoip_location($chk_ip, true);
    $isp      = get_geoip_location_data($chk_ip, "isp");
    $agent    = $_SERVER["HTTP_USER_AGENT"];
    $logmsg   = "[$lognowd] - $chk_ip - $location - $isp - $agent\n";
    @file_put_contents($logfile, $logmsg, FILE_APPEND);
    
    $logmsg   = "           ! $chk_ip - blocked after $bftries attempts.\n";
    @file_put_contents($logfile, $logmsg, FILE_APPEND);
    
    $mem_cache->set($mem_key, $bf_hits + 1, 0, $mem_ttl);
    throw_fake_401();
}

$bf_hits++;
if( $bf_hits > $bftries ) throw_fake_401();
$mem_cache->set($mem_key, $bf_hits, 0, $mem_ttl);

#=========
#endregion

$current_module->load_extensions("login", "bootstrap");

#=====================#
# Account validations #
#=====================#

if( stristr($_POST["user_name"], "@") !== false )
    die("ERROR_NO_EMAIL_AS_USER_NAME");

if( trim($_POST["user_name"]) == "" || trim($_POST["password"])  == "" )
    die("ERROR_MISSING_PARAMS");

$account = new account(trim(stripslashes($_POST["user_name"])));

if( ! $account->_exists )
    die("ERROR_ACCOUNT_UNEXISTENT");

if( $account->state == "new" )
{
    $account->send_new_account_confirmation_email();
    die("ERROR_UNCONFIRMED_ACCOUNT");
}

if( $account->state != "enabled" )
    die("ERROR_ACCOUNT_DISABLED");

#region IPs whitelist checks
#===========================

$ips_whitelist = $account->engine_prefs["@accounts:ips_whitelist"];
if( ! empty($ips_whitelist) )
{
    $ip    = get_user_ip();
    $lines = explode("\n", $ips_whitelist);
    $found = false;
    foreach($lines as $line)
    {
        $listed_ip = trim($line);
        if( empty($listed_ip) ) continue;
        if( substr($listed_ip, 0, 1) == "#" ) continue;
        
        if( stristr($listed_ip, "*") )
        {
            $pattern = str_replace(".", "\\.", $listed_ip);
            $pattern = str_replace("*", ".*",  $pattern);
            
            if(preg_match("/$pattern/", $ip) )
            {
                $found = true;
                break;
            }
        }
        else
        {
            if( $ip == $listed_ip )
            {
                $found = true;
                break;
            }
        }
    }
    
    if( ! $found )
    {
        $current_module->load_extensions("login", "after_whitelist_check_fail");
        die("ERROR_IP_NOT_IN_WHITELIST");
    }
}

#=========
#endregion

$current_module->load_extensions("login", "pre_validations");

if( md5(trim(stripslashes($_POST["password"]))) != $account->password )
    die("ERROR_WRONG_PASSWORD");

# 2fa checks
if( $account->has_2fa_enabled() )
{
    $tfa_token = trim(stripslashes($_POST["tfa_token"]));
    if( empty($tfa_token) ) die("ERROR_MISSING_2FA_TOKEN");
    if( ! is_numeric($tfa_token) ) die("ERROR_INVALID_2FA_TOKEN");
    
    if( ! $account->validate_2fa_token($tfa_token) ) die("ERROR_2FA_VALIDATION_FAILED");
}

if( $settings->get("engine.enabled") != "true" &&
    ! $account->_is_admin )
    die("ERROR_ENGINE_DISABLED");

#==========================================#
# Session opening and new device detection #
#==========================================#

$device = new device($account->id_account);

if( $settings->get("modules:accounts.enforce_device_registration") != "true" )
{
    if( ! $device->_exists )
    {
        $device->set_new($account);
        $device->state = "enabled";
        $device->save();
        $device_return = "OK";
    }
    else
    {
        $device->ping();
        $device_return = "OK";
    }
}
else # $settings->get("modules:accounts.enforce_device_registration") == "true"
{
    if( ! $device->_exists )
    {
        $device->set_new($account);
        $device->save();
        $device->send_auth_token($account);
        $device_return = "UNREGISTERED";
    }
    else
    {
        switch( $device->state )
        {
            case "disabled":
                die("ERROR_DEVICE_DISABLED");
                
            case "enabled":
                $device->ping();
                $device_return = "OK";
                break;
            
            case "unregistered":
            default:
                $device->send_auth_token($account);
                $device_return = "UNREGISTERED";
                break;
        }
    }
}

$account->open_session($device);

$mem_cache->delete($mem_key);

if( ! empty($_REQUEST["redir_url"]) )
{
    header("Location: " . $_REQUEST["redir_url"]);
    die("<a href='".$_REQUEST["redir_url"]."'>{$language->click_here_to_continue}</a>");
} # end if

$goto = empty($_REQUEST["goto"]) ? "/" : $_REQUEST["goto"];

$current_module->load_extensions("login", "before_successful_output");

echo "OK\t{$account->user_name}\t{$device_return}\t{$goto}";
