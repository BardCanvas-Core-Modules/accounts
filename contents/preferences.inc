<?php
/**
 * @var array             $errors
 * @var array             $messages
 * @var config            $config
 * @var \SimpleXMLElement $language
 */

use hng2_base\config;

include_once "{$current_module->abspath}/2fa/totp.php";

$editable_prefs = $config->globals["editable_prefs_collection"];
$account_prefs  = $account->get_editable_prefs();

if( $settings->get("modules:accounts.use_2fa") != "true" )
    unset($editable_prefs["{$current_module->language->engine_prefs->title}"]);

$url = "{$_SERVER["PHP_SELF"]}?wasuuup=" . md5(mt_rand(1, 65535))
     . ( empty($_REQUEST["bcm_platform"]     ) ? "" : "&bcm_platform="      . $_REQUEST["bcm_platform"]      )
     . ( empty($_REQUEST["bcm_version"]      ) ? "" : "&bcm_version="       . $_REQUEST["bcm_version"]       )
     . ( empty($_REQUEST["bcm_access_token"] ) ? "" : "&bcm_access_token="  . $_REQUEST["bcm_access_token"]  )
     . ( empty($_REQUEST["bcm_force_session"]) ? "" : "&bcm_force_session=" . $_REQUEST["bcm_force_session"] )
     . ( empty($_REQUEST["bcm_force_session"]) ? "" : "&bcm_output_type="   . "HTML"                         )
;
?>

<h1 class="clearfix">
    <button class="pull-right" onclick="location.href = '<?= $url ?>'">
        <span class="fa fa-refresh"></span>
        <?= $language->reload ?>
    </button>
    <?= $current_module->language->page_titles->preferences ?>
</h1>

<? if( empty($editable_prefs) ): ?>
    <div class="framed_content state_highlight">
        <span class="fa fa-info-circle"></span>
        <?= $current_module->language->no_engine_prefs ?>
    </div>
    
    <? return; ?>
<? endif; ?>

<!-- CodeMirror -->
<script                 src="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/lib/codemirror.js"></script>
<link rel="stylesheet" href="<?=$config->full_root_path?>/lib/CodeMirror-5.16.2/lib/codemirror.css">
<style type="text/css">
    .CodeMirror {
        border: 1px solid silver;
        height: auto;
        font-family: 'Lucida Console', 'Courier new', Courier, monospace !important; font-size: 8pt;
        /*max-width: 99%;*/
    }
</style>
<script type="text/javascript">
    $(document).ready(function()
    {
        $('textarea.CodeMirror').each(function()
        {
            var element = $(this).get(0);
            
            CodeMirror.fromTextArea(element, {
                viewPortMargin: Infinity,
                lineNumbers:    true
            });
        });
    });
</script>

<!-- 2FA -->
<script type="text/javascript">
    
    function open_2fa_configuration_dialog()
    {
        $('#2fa_creation_dialog').dialog('open');
    }
    
    function validate_and_save_2fa_settings()
    {
        var $dialog = $('#2fa_creation_dialog');
        var url     = $_FULL_ROOT_PATH + '/accounts/2fa/toolbox.php';
        var params  = {
            save_new_key: $dialog.find('#2fa_raw_secret').attr('data-value'),
            token:        $dialog.find('#2fa_token_input').val().trim()
        };
        
        $dialog.closest('.ui-dialog').block(blockUI_default_params);
        $.get(url, params, function(response)
        {
            if( response !== 'OK' )
            {
                $dialog.closest('.ui-dialog').unblock();
                
                throw_notification(response, 'warning', 4000);
                return;
            }
            
            $('#preferences').submit();
        });
    }
    
    function disable_2fa()
    {
        var url     = $_FULL_ROOT_PATH + '/accounts/2fa/toolbox.php';
        var params  = {
            disable: 'true',
            token:   $('#tfa_disabling_token_input').val().trim()
        };
        
        $.blockUI(blockUI_default_params);
        $.get(url, params, function(response)
        {
            if( response !== 'OK' )
            {
                $.unblockUI();
                
                throw_notification(response, 'warning', 4000);
                return;
            }
            
            $('#preferences').submit();
        });
    }
    
    $(document).ready(function()
    {
        var $dialog = $('#2fa_creation_dialog');
        var cancel  = $dialog.attr('data-cancel-button');
        var submit  = $dialog.attr('data-ok-button');
        var width   = $(window).width()  - 20; if( width  > 440 ) width  = 420;
        var height  = $(window).height() - 20; if( height > 700 ) height = 680;
        
        $dialog.dialog({
            autoOpen: false,
            modal:    true,
            width:    width,
            height:   height,
            open:     function() {
                          var $img = $dialog.find('#2fa_qrcode');
                          if( $img.attr('loaded') ) return;
                          
                          var url  = $img.attr('data-src');
                          $img.attr('src', url);
                          $img.attr('loaded', 'true');
                      },
            buttons:  [
                {
                    text:  submit,
                    icons: { primary: "ui-icon-check" },
                    click: function() { validate_and_save_2fa_settings(); }
                },
                {
                    text:  cancel,
                    icons: { primary: "ui-icon-cancel" },
                    click: function() { $(this).dialog( "close" ); }
                }
            ]
        });
    });
</script>

<? $current_module->load_extensions("prefs_editor", "script_preloads"); ?>

<form method="post" id="preferences" action="<?= $_SERVER["PHP_SELF"] ?>?wasuuup=<?=md5(mt_rand(1,65535))?>">
    
    <input type="hidden" name="bcm_platform"      value="<?= $_REQUEST["bcm_platform"] ?>">
    <input type="hidden" name="bcm_version"       value="<?= $_REQUEST["bcm_version"] ?>">
    <input type="hidden" name="bcm_access_token"  value="<?= $_REQUEST["bcm_access_token"] ?>">
    <input type="hidden" name="bcm_force_session" value="<?= $_REQUEST["bcm_force_session"] ?>">
    <input type="hidden" name="bcm_output_type"   value="<? if( ! empty($_REQUEST["bcm_force_session"]) ) echo "HTML"; ?>">
    
    <?
    $sanitized_list = array();
    ksort($editable_prefs);
    foreach($editable_prefs as $title => $group_data)
    {
        foreach($group_data as $key => $specs)
        {
            if( isset($specs["min_account_level"]) )
                if( $account->level < $specs["min_account_level"] )
                    continue;
            
            $sanitized_list[$title][$key] = $specs;
        }
    }
    $editable_prefs = $sanitized_list;
    ?>
    <? foreach($editable_prefs as $title => $group_data): ?>
        
        <div class="table_wrapper">
            <table class="settings_group nav_table no_highlight" style="margin-bottom: 20px;">
                
                <thead>
                <tr>
                    <th colspan="2"><?= $title ?></th>
                </tr>
                </thead>
                
                <tbody>
                <? foreach($group_data as $key => $specs):
                    
                    if( isset($specs["min_account_level"]) )
                        if( $account->level < $specs["min_account_level"] )
                            continue;
                    
                    /** @var \SimpleXMLElement $specs */
                    
                    $class = "";
                    if($specs["is_locked"] == "true") $class .= " disabled";
                    if($specs["highlight_class"])     $class .= " {$specs["highlight_class"]}";
                    
                    $full_layout = isset($specs->options->full_layout);
                    ?>
                    
                    <tr class="<?= $class ?>">
                        <td class="details" <? if($full_layout) echo "colspan='2'"; ?> >
                            <div class="principal_data">
                                <? if($specs["is_locked"] == "true") echo "<span class='fa fa-lock'></span>" ?>
                                <?= $specs->title ?>
                                <? if( $account->_is_admin ): ?>
                                    <span class="framed_content inlined fixed_font" style="font-weight: normal;">
                                        <?= $key ?>
                                    </span>
                                <? endif; ?>
                            </div>
                            <blockquote>
                                <?= $specs->description ?>
                            </blockquote>
                        <? if( ! $full_layout ): ?></td><? endif; ?>
                        <? if( ! $full_layout ): ?><td class="control" nowrap><? endif; ?>
                            <?
                            $selector_default = trim($specs["selector_default"]);
                            $value = $account_prefs[$key];
                            if( $value == "" && ! empty($selector_default) ) $value = $selector_default;
                                
                            if($specs["is_locked"] != "true")
                                render_pref_editor($specs->type, $key, $value, $specs);
                            else
                                echo "<div class='framed_content disabled'>" 
                                    . ($value == "" ? "&mdash;" : $value) 
                                    . "</div>";
                            ?>
                        </td>
                    </tr>
                <? endforeach; ?>
                </tbody>
            </table>
            
        </div>
        
    <? endforeach; ?>
    
    <p align="right">
        <button type="submit" class="big_form_submit_button">
            <span class="fa fa-check"></span>
            <?= $language->save ?>
        </button>
    </p>
    
</form>

<?
$totp       = new totp();
$new_secret = $totp->createSecret();
$enc_secret = three_layer_encrypt(
    $new_secret, $config->website_key, $account->id_account, $account->creation_date
);
?>
<div id="2fa_creation_dialog" title="<?= $current_module->language->tfa->dialog->title ?>"
     data-ok-button="<?= $current_module->language->tfa->dialog->save ?>"
     data-cancel-button="<?= $language->cancel ?>"
     style="display: none; font-family: arial, helvetica, sans-serif; font-size: 12pt; line-height: normal;">
    
    <input type='text' size='1' style='position: relative; top: -100px; width: 1px; height: 1px;' />
    <p style="margin-top: 0">
        <?= $current_module->language->tfa->dialog->info1 ?>
    </p>
    
    <p class="aligncenter">
        <img id="2fa_qrcode" style="width: 200px; height: 200px;"
             src="<?= $config->full_root_path ?>/media/missing_image.png"
             data-src="<?= $current_module->get_url() ?>/2fa/toolbox.php?new_secret_qc=<?= urlencode($enc_secret) ?>"><br>
        
        <input type="text" class="aligncenter" readonly value="<?= $new_secret ?>"
               id="2fa_raw_secret" data-value="<?= $enc_secret ?>"
               style="font-size: large; margin-top: 5px; width: 200px;">
    </p>
    
    <div class="framed_content state_highlight">
        <i class="fa fa-warning"></i>
        <?= $current_module->language->tfa->dialog->info3 ?>
    </div>
    
    <p>
        <?= $current_module->language->tfa->dialog->info2 ?>
    </p>
    <p class="aligncenter" style="margin-bottom: 0">
        <input type="text" id="2fa_token_input" autocomplete="off"
               style="font-size: 18pt; text-align: center"
               maxlength="6" placeholder="000000">
    </p>
    
</div>

<?
$generation_index = 0;
function render_pref_editor($type, $name, $value, $specs = null)
{
    global $language, $generation_index, $config;
    
    $generation_index++;
    $type = trim($type);
    
    if( $type == "list" )
    {
        echo "<select name='engine_prefs[{$name}]'>";
        foreach($specs->options->option as $option)
        {
            $selected = trim($option["value"]) == $value ? "selected" : "";
            echo "<option {$selected} value='{$option["value"]}'>{$option}</option>";
        }
        echo "</select>";
        
        return;
    }
    
    if( $type == "boolean" )
    {
        $value     = $value == "true" ? "true" : "false";
        $style_on  = $value == "true" ? "" : "display: none;";
        $style_off = $value != "true" ? "" : "display: none;";
        echo "
            {$language->off}
            <span class='fa-pseudo-switch' data-value-on='true' data-value-off='false' onclick='toggle_fa_pseudo_switch(this);'>
                <input type='hidden' name='engine_prefs[$name]' value='$value'>
                <span class='toggler toggle-on  fa fa-toggle-on'  style='$style_on'></span>
                <span class='toggler toggle-off fa fa-toggle-off' style='$style_off'></span>
            </span>
            {$language->on}
        ";
        
        return;
    }
    
    if( in_array($type, array("text", "email")) )
    {
        $value = htmlspecialchars($value);
        echo "<input " . " type='{$type}' name='engine_prefs[{$name}]' value='{$value}'>";
        
        return;
    }
    
    if( $type == "yes/no" )
    {
        $value   = $value == "true" ? "true" : "false";
        $sel_on  = $value == "true" ? "checked" : "";
        $sel_off = $value != "true" ? "checked" : "";
        echo "
            <label><input type='radio' $sel_on  name='engine_prefs[{$name}]' value='true'>{$language->yes}</label>
            <label><input type='radio' $sel_off name='engine_prefs[{$name}]' value='false'>{$language->no}</label>
        ";
        
        return;
    }
    
    if( $type == "number" )
    {
        echo "
            <input type='number' name='engine_prefs[{$name}]' value='{$value}' style='text-align: center;'
                   min='{$specs->options->min}' max='{$specs->options->max}'>
        ";
        
        return;
    }
    
    if( $type == "code" )
    {
        echo "
            <textarea class='CodeMirror' id='ta_$generation_index' name='engine_prefs[{$name}]'>" . htmlspecialchars($value) . "</textarea>
        ";
        
        return;
    }
    
    if( $type == "external" )
    {
        $id  = "ee_" . uniqid();
        $url = "{$config->full_root_path}/{$specs->options->url}"
             . "?wasuuup=" . md5(mt_rand(1, 65535))
             . ( empty($_REQUEST["bcm_platform"]     ) ? "" : "&bcm_platform="      . $_REQUEST["bcm_platform"]      )
             . ( empty($_REQUEST["bcm_version"]      ) ? "" : "&bcm_version="       . $_REQUEST["bcm_version"]       )
             . ( empty($_REQUEST["bcm_access_token"] ) ? "" : "&bcm_access_token="  . $_REQUEST["bcm_access_token"]  )
             . ( empty($_REQUEST["bcm_force_session"]) ? "" : "&bcm_force_session=" . $_REQUEST["bcm_force_session"] )
             . ( empty($_REQUEST["bcm_force_session"]) ? "" : "&bcm_output_type="   . "HTML"                         )
        ;
        echo "
            <div id='$id' class='external_editor_target' data-pref-name='{$name}'>
                <div class='framed_content aligncenter' style='margin: 0;'>
                    <i class='fa fa-spinner fa-pulse fa-lg'></i>
                </div>
            </div>
            
            <script type='text/javascript'>
                $(document).ready(function() { $('#{$id}').load('{$url}'); });
            </script>
        ";
        
        return;
    }
    
    # Default: textarea
    echo "
        <textarea class='expandible_textarea' name='engine_prefs[{$name}]'>" . htmlspecialchars($value) . "</textarea>
    ";
}
