<?php
/**
 * Account preferences page
 *
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

include "../config.php";
include "../includes/bootstrap.inc";

if( ! empty($_POST["engine_prefs"]) )
{
    $messages = array();
    
    # echo "<pre>" . print_r($account->engine_prefs, true) . "</pre>";
    foreach($_POST["engine_prefs"] as $key => $val)
    {
        # echo "$key => $val<br>";
    
        $val = trim(stripslashes($val));
        if( $val != $account->engine_prefs[$key] )
        {
            $account->set_engine_pref($key, $val);
            $messages[] = replace_escaped_vars(
                $current_module->language->pref_saved_ok,
                '{$key}',
                ucwords(str_replace("_", " ", end(explode(":", $key))))
            );
        }
    }
    
    if( count($messages) > 0 )
        send_notification($account->id_account, "success", implode("\n", $messages));
}

$template->page_contents_include = "preferences.inc";
$template->set_page_title($current_module->language->page_titles->preferences);
include "{$template->abspath}/main.php";
