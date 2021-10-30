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
    
    $current_module->load_extensions("prefs_editor", "before_saving");
    
    if( count($messages) == 0 )
    {
        foreach($_POST["engine_prefs"] as $key => $val)
        {
            if( is_string($val) ) $val = trim(stripslashes($val));
            
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
        
        $current_module->load_extensions("prefs_editor", "after_saving");
    }
    
    if( count($messages) > 0 )
        send_notification($account->id_account, "success", implode("\n", $messages));
}

$template->page_contents_include = "preferences.inc";
$template->set("page_tag", "user_home");
$template->set_page_title($current_module->language->page_titles->preferences);
include "{$template->abspath}/main.php";
