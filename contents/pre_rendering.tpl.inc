<?php

if( ! $account->_exists ) return;
if( ! empty($account->email) ) return;
if( ! empty($account->alt_email) ) return;

$now      = date("Y-m-d H:i:s");
$boundary = date("Y-m-d H:i:s", strtotime("now - 24 hours"));

if( isset($account->engine_prefs["_no_email_reminder_sent"]) )
    if( $account->engine_prefs["_no_email_reminder_sent"] >= $boundary )
        return;

$message = replace_escaped_objects($this_module->language->missing_email, array(
    '{$url}' => "{$this_module->get_url()}/edit_account.php"
));
send_notification($account->id_account, "error", $message);

$account->set_engine_pref("_no_email_reminder_sent", $now);
