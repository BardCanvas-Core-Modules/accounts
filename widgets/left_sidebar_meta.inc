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

global $config, $language, $account;

$items = array();
$display_name = convert_emojis($account->display_name);

$items[] = "
    <a class='item' href='{$config->full_root_path}/user/{$account->user_name}'>
        <img class='avatar' src='{$account->get_avatar_url()}'>
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

return implode("\n", $items);