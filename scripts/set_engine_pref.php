<?php
/**
 * Account engine pref saver
 *
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @param string "id_account"
 * @param string "key"
 * @param string "value"
 * 
 * @return string "OK" on success | error message
 */

use hng2_base\account;

include "../../config.php";
include "../../includes/bootstrap.inc";

$_current_page_requires_login = true;
if( ! $account->_exists ) die("OK");
if( $account->state != "enabled" ) throw_fake_401();
if( $account->level < $config::NEWCOMER_USER_LEVEL ) throw_fake_401();

$key = trim(stripslashes($_REQUEST["key"]));
if( empty($key) ) die($current_module->language->errors->prefs_setting->empty_key);

$val = trim(stripslashes($_REQUEST["value"]));

$id_account = $_REQUEST["id_account"] + 0;

try
{
    check_sql_injection(array($key, $val, $id_account));
}
catch(\Exception $e)
{
    throw_fake_501();
}

if( ! empty($id_account) && $id_account != $account->id_account )
{
    if( ! $account->_is_admin )
        die( $language->errors->access_denied );
    
    $user_account = new account($id_account);
    if( ! $user_account->_exists ) die($current_module->language->admin->record_nav->action_messages->target_not_exists);
    
    if( $key == "granted_admin_to_modules" )
        $previous_grants = $user_account->engine_prefs["granted_admin_to_modules"];
    
    $user_account->set_engine_pref($key, $val);
    
    if( $key == "granted_admin_to_modules" )
    {
        if( empty($previous_grants) ) $previous_grants = array();
        else                          $previous_grants = preg_split('/,\s*/', $previous_grants);
        $previous_grants = empty($previous_grants) ? "none" : implode(", ", $previous_grants);
        $final_grants    = $val;
        if( empty($final_grants) ) $final_grants = "none";
        
        if( $previous_grants != $final_grants )
        {
            $ip   = get_remote_address();
            $host = @gethostbyaddr($ip); if(empty($host)) $host = $ip;
            $loc  = get_geoip_location($ip);
            $isp  = get_geoip_isp($ip);
            $agnt = $_SERVER["HTTP_USER_AGENT"];
            
            $fields = array(
                '{$target_display_name}' => $user_account->display_name,
                '{$user_display_name}'   => $account->display_name,
                '{$target_id_account}'   => $user_account->id_account,
                '{$target_home_link}'    => "{$config->full_root_url}/user/{$user_account->user_name}",
                '{$target_username}'     => $user_account->user_name,
                '{$user_id_account}'     => $account->id_account,
                '{$user_home_link}'      => "{$config->full_root_url}/user/{$account->user_name}",
                '{$user_username}'       => $account->user_name,
                '{$previous_grants}'     => $previous_grants,
                '{$final_grants}'        => $final_grants,
                '{$ip}'                  => $ip,
                '{$host}'                => $host,
                '{$loc}'                 => $loc,
                '{$isp}'                 => $isp,
                '{$agnt}'                => $agnt,
            );
            
            $subject = replace_escaped_objects($current_module->language->grants_changed->subject, $fields);
            $body    = unindent(replace_escaped_objects($current_module->language->grants_changed->body, $fields));
            broadcast_mail_between_levels($config::COADMIN_USER_LEVEL, $config::ADMIN_USER_LEVEL, $subject, $body);
        }
    }
    
    die("OK");
}

$config->add_to_restricted_engine_prefs("/granted_admin_to_modules/i");
if( ! $account->_is_admin && $config->is_engine_pref_restricted($key) )
    die($current_module->language->admin->record_nav->action_messages->restricted_pref);
    
$account->set_engine_pref($key, $val);
echo "OK";
