<?php
/**
 * Send notification to online users
 *
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * $_POST params:
 * @param string "type"
 * @param string "message"
 *
 * @return string "OK" on success | error message
 */

use hng2_base\accounts_repository;

include "../../config.php";
include "../../includes/bootstrap.inc";

if( ! $account->has_admin_rights_to_module("accounts") ) throw_fake_401();

$type    = trim(stripslashes($_POST["type"]));
$message = trim(stripslashes($_POST["message"]));

if( empty($type) )    die($current_module->language->notify_online_users->messages->invalid_type);
if( empty($message) ) die($current_module->language->notify_online_users->messages->empty_message);

if( ! in_array($type, array("information", "success", "alert", "warning", "error")) )
    die($current_module->language->notify_online_users->messages->invalid_type);

$repository = new accounts_repository();
$users      = $repository->get_online_users_list(10);
if( empty($users) ) die($current_module->language->notify_online_users->messages->none_online);

$message = nl2br($message);
send_notification( $account->id_account, $type, convert_emojis($message) );
foreach($users as $user_data)
    send_notification( $user_data->id_account, $type, convert_emojis($message) );

echo "OK";
