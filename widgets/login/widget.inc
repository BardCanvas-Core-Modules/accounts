<?php
/**
 * Login for right sidebar
 * Called as include() from within a function
 *
 * @package    HNG2
 * @subpackage accounts
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

global $account, $config, $language;

if( $account->_exists )
    return "
        <div class='sidebar_user_info meta_box clearfix'>
            <img class='user_avatar' src='{$account->get_avatar_url()}'>
            <div class='details'>
                <div class='meta_section upper'>
                    <a class='meta_field' href='{$config->full_root_path}/user/{$account->user_name}'>{$account->get_processed_display_name()}</a>
                </div>
                <div class='meta_section'>
                    <a class='meta_field dimmed' href='{$config->full_root_path}/accounts/scripts/logout.php'>
                        <span class='fa fa-sign-out'></span>
                        {$this_module->language->logout}
                    </a>
                </div>
            </div>
        </div>
    ";

ob_start(); ?>

<div id="sidebar_login">
    <form name="sidebar_login_form" id="sidebar_login_form" method="post"
          action="<?=$config->full_root_path?>/accounts/scripts/login.php">
        
        <div class="field">
            <div class="caption"><?= $this_module->language->user_name ?></div>
            <div class="input"><input type="text" name="user_name"></div>
        </div>
        
        <div class="field">
            <div class="caption"><?= $this_module->language->password ?></div>
            <div class="input"><input type="password" name="password"></div>
        </div>
        
        <div class="field">
            <span class="pseudo_link" onclick="$('#login_dialog').dialog('close'); reset_password();">
                <span class="fa fa-key"></span>
                <?= $this_module->language->reset_password ?>
            </span>
        </div>
        
        <div class="field alignright">
            <button type="submit">
                <span class="fa fa-sign-in"></span>
                <?= $this_module->language->login ?>
            </button>
        </div>
    </form>
</div>

<div id="sidebar_login_targetarea" style="display: none;"></div>

<script type="text/javascript">
    
    $('#sidebar_login_form').ajaxForm({
        target:        '#sidebar_login_targetarea',
        beforeSubmit:  validate_sidebar_login_form,
        success:       process_sidebar_login_result
    });
    
    function validate_sidebar_login_form()
    {
        var $login_form = $('#sidebar_login_form');
        if( $login_form.find('input[name="user_name"]').val().trim() == "" ||
            $login_form.find('input[name="user_name"]').val().trim() == "" ) {
            alert( $('#login_errors').find('.invalid_login_info').text().replace(/\n\s+/g, ' ') );
            return false;
        }
        
        return true;
    }
    
    function process_sidebar_login_result(result)
    {
        if( result.indexOf('ERROR') < 0 )
        {
            // result > username > device_message > redirect
            parts = result.split('\t');
            
            // Let's show the info and logout button
            $('.login').hide();
            $('#loggedin_username').text(parts[1]);
            $('.logout').show();
            
            // Let's check if we need to alert about the new device
            if( parts[2] != "OK" )
            {
                $('#loggedin_icon').hide();
                $('#loggedin_icon_locked').show();
                alert( $('#device_messages .' + parts[2]).text().replace(/\n\s+/g, ' ') );
                return;
            }
            
            if( parts[3] != '' ) location.href = $_FULL_ROOT_PATH + parts[3];
            
            return;
        }
        
        alert( $('#login_errors .' + result).text().replace(/\n\s+/g, ' ') );
    }
</script>

<? return ob_get_clean();
