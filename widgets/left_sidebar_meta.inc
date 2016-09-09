<?php
/**
 * User meta for left sidebar
 * Called as include() from within a function
 *
 * @package    HNG2
 * @subpackage account
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Trailing vars:
 * @var module[]          $modules
 * @var module            $this_module    widgets_manager
 * @var template          $template       The caller
 * @var \SimpleXMLElement $widget
 * @var string            $seed
 * @var string            $data_key
 *
 * @return string
 */

use hng2_base\module;
use hng2_base\template;

global $config, $language, $account, $settings;

$items = array();

if( ! $account->_exists )
{
    $items[] = "
        <span class='item important pseudo_link' onclick='show_login_form()'>
            <span class='fa fa-sign-in'></span>
            {$this_module->language->login}
        </span>
    ";
    if( $settings->get("modules:accounts.register_enabled") != "false" )
        $items[] = "
            <a class='item important critical pseudo_link' href='{$config->full_root_path}/accounts/register.php'>
               <span class='fa fa-pencil'></span>
               {$this_module->language->create_account}
            </a>
        ";
}
else
{
    $display_name = convert_emojis($account->display_name);
    $items[] = "
        <a class='item user_name' href='{$config->full_root_path}/user/{$account->user_name}'
           style='background-image: url({$account->get_avatar_url()})'>
            {$display_name}
        </a>
    ";
    
    $selected = stristr($_SERVER["PHP_SELF"], "/accounts/edit_account.php") !== false ? "selected" : "";
    $items[] = "
        <a class='item {$selected}' href='{$config->full_root_path}/accounts/edit_account.php'>
            <span class='fa fa-pencil'></span>
            {$this_module->language->widgets->meta->edit_profile}
        </a>
    ";
    $items[] = "
        <a class='item' href='{$config->full_root_path}/accounts/scripts/logout.php'>
            <span class='fa fa-sign-out'></span>
            {$this_module->language->logout}
        </a>
    ";
}

return implode("\n", $items);
