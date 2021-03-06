<?php
/**
 * Accounts header buttons
 *
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *             
 * @var account  $account
 * @var settings $settings
 * @var config   $config
 * @var template $template
 */

use hng2_base\account;
use hng2_base\config;
use hng2_base\settings;
use hng2_base\template;

if( ! $account->_exists )
{
    if( $settings->get("modules:accounts.register_enabled") != "false" )
        $template->add_menu_item(
            "_____login_____",
            "
            <a class='main_menu_item always_visible pull-right' data-module='accounts' href='{$config->full_root_path}/accounts/register.php'>
               <span class='fa fa-pencil'></span>
               {$this_module->language->register}
            </a>
            ",
            950
        );
    
    $template->add_menu_item(
        "_____login_____",
        "
        <span class='main_menu_item always_visible pull-right pseudo_link' id='header_login_button' data-module='accounts' onclick='show_login_form()'>
           <span class='fa fa-sign-in'></span>
           {$this_module->language->login}
        </span>
        ",
        950
    );
}
else
{
    if( $account->has_admin_rights_to_module("accounts") && $modules["messaging"]->enabled )
        $template->add_menu_item(
            "notify_online_users",
            "
            <span class='main_menu_item pseudo_link highlighted always_visible pull-right' data-module='accounts' onclick='notify_online_users()'
                  title='{$this_module->language->notify_online_users->title}'>
               <span class='fa fa-send fa-fw'></span>
            </span>
            ",
            960
        );
    
    $added_class = preg_match('#/accounts/edit_account.php|/accounts/devices.php|/accounts/preferences.php#i', $_SERVER["PHP_SELF"]) ? "current" : "";
    $locked      = $account->_is_locked ? "fa-lock" : "fa-info-circle";
    
    $display_name = convert_emojis($account->display_name);
    $template->add_menu_item(
        "_____login_____",
        "
        <span class='main_menu_item {$added_class} always_visible pull-right is_submenu_trigger' id='loggedin_username' 
              data-module='accounts' style='cursor: pointer;' data-submenu='#user_menu_items'>
            <span id='loggedin_icon' class='fa fa-border {$locked}' style='color: transparent; background: url({$account->get_avatar_url()}) center center no-repeat; background-size: cover'></span>
            <span id='loggedin_icon_locked' class='fa fa-border fa-lock' style='display: none;'></span>
            <span id='logedin_user_name'>{$display_name}</span>
            <span class='menu_toggle'>
                <span class='fa fa-caret-down'></span>
                <span class='fa fa-caret-up' style='display: none;'></span>
            </span>
        </span>
        ",
        950
    );
}
