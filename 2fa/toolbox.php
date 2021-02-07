<?php
/**
 * 2fa toolbox
 *
 * Usage:
 * 
 * • Render the QRCODE of a new secret key:
 *   > toolbox.php?new_secret_qc=<encrypted new secret key>
 *   Returns the PNG output.
 * 
 * • Check a new secret key against its token and save it:
 *   > toolbox.php?save_new_key=<encrypted new secret key>&token=<app-generated token>
 *   Returns "OK" or an error message in plain text.
 * 
 * • Disable 2FA for the account:
 *   > toolbox.php?disable=true&token=<app-generated token>
 * 
 * @package    HNG2
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * @subpackage accounts
 * 
 * $_GET params:
 * @param string "new_secret_qc" Encrypted with website key + account id + account creation date
 * @param string "save_new_key"  Encrypted with website key + account id + account creation date
 * @param int    "token"         App-generated token to check against the new key
 */

include "../../config.php";
include "../../includes/bootstrap.inc";
include __DIR__ . "/totp.php";
include ROOTPATH . "/lib/phpqrcode/qrlib.php";

if( ! $account->_exists ) throw_fake_401();
if( $account->state != "enabled" ) throw_fake_401();

#
# New secret QRcode
#

if( ! empty($_GET["new_secret_qc"]) )
{
    $enc_secret = trim(stripslashes($_GET["new_secret_qc"]));
    $raw_secret = three_layer_decrypt($enc_secret, $config->website_key, $account->id_account, $account->creation_date);
    $qrcode_str = sprintf(
        "otpauth://totp/%s?secret=%s",
        rawurlencode("{$account->user_name} @ {$settings->get('engine.website_name')}"),
        urlencode($raw_secret)
    );
    QRcode::png($qrcode_str, false, QR_ECLEVEL_L, 6);
    exit;
}

#
# Check new secret key and validate against incoming token
#

if( ! empty($_GET["save_new_key"]) )
{
    header("Content-Type: text/plain; charset=UTF-8");
    $enc_secret  = trim(stripslashes($_GET["save_new_key"]));
    $raw_secret  = three_layer_decrypt($enc_secret, $config->website_key, $account->id_account, $account->creation_date);
    $input_token = trim(stripslashes($_GET["token"]));
    
    if( empty($input_token) ) die($current_module->language->tfa->missing_token);
    if( ! is_numeric($input_token) ) die($current_module->language->tfa->invalid_token);
    
    $totp = new totp();
    $res  = $totp->verifyCode($raw_secret, $input_token);
    
    if( ! $res ) die($current_module->language->tfa->verification_failed);
    
    $account->set_engine_pref("@accounts:2fa_secret", $enc_secret);
    send_notification($account->id_account, "success", $current_module->language->tfa->verification_passed);
    die("OK");
}

if( $_GET["disable"] == "true" )
{
    $input_token = trim(stripslashes($_GET["token"]));
    
    if( empty($input_token) ) die($current_module->language->tfa->missing_token);
    if( ! is_numeric($input_token) ) die($current_module->language->tfa->invalid_token);
    
    $enc_secret = $account->engine_prefs["@accounts:2fa_secret"];
    if( empty($enc_secret) ) die("OK");
    
    $raw_secret = three_layer_decrypt($enc_secret, $config->website_key, $account->id_account, $account->creation_date);
    
    $totp = new totp();
    $res  = $totp->verifyCode($raw_secret, $input_token);
    
    if( ! $res ) die($current_module->language->tfa->verification_failed2);
    
    $account->set_engine_pref("@accounts:2fa_secret", "");
    send_notification($account->id_account, "success", $current_module->language->tfa->disabled_ok);
    die("OK");
}

throw_fake_501();
