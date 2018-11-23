<?php
/**
 * @var config            $config
 * @var \SimpleXMLElement $language
 * @var module            $current_module
 * @var array             $_messages
 * @var array             $_errors
 * @var array             $_country_list
 * @var string            $_current_user_country
 * @var string            $_cancelation_redirect
 * @var string            $_submit_button_caption
 * @var string            $_form_title
 * @var bool              $_include_account_id
 * @var account           $xaccount
 */

use hng2_base\account;
use hng2_base\config;
use hng2_base\module;
?>

<link rel="stylesheet" type="text/css" href="<?= $config->full_root_path ?>/lib/jscalendar/calendar-blue.css">
<script type="text/javascript"          src="<?= $config->full_root_path ?>/lib/jscalendar/calendar.js"></script>
<script type="text/javascript"          src="<?= $config->full_root_path ?>/lib/jscalendar/lang/calendar-<?= $language->info->iso ?>.js"></script>
<script type="text/javascript"          src="<?= $config->full_root_path ?>/lib/jscalendar/calendar-setup.js"></script>
<script type="text/javascript"          src="<?=$config->full_root_path?>/lib/jquery.exptextarea.js"></script>
<script type="text/javascript"> $(document).ready(function() { $('.expandible_textarea').expandingTextArea(); }); </script>

<h1 class="clearfix">
    <? if( ! empty($_cancelation_redirect) ): ?>
        <button class="pull-right" onclick="location.href='<?= $_cancelation_redirect ?>'">
            <span class="fa fa-reply"></span>
            <?= $language->return ?>
        </button>
    <? endif; ?>
    
    <?= empty($_form_title) ? $current_module->language->edit_account_form->form_title : $_form_title ?>
</h1>

<? if($_messages): ?>
    <div class="framed_content state_ok">
        <? foreach($_messages as $_message): ?>
            <span class="fa fa-info-circle"></span><?= $_message ?><br>
        <? endforeach; ?>
    </div>
<? endif; ?>

<? if($_errors): ?>
    <div class="framed_content state_ko">
        <span class="fa fa-warning"></span>
        <?= $current_module->language->errors->registration->errors_found_in_form->pre ?>
        <ul>
            <? foreach($_errors as $_error): ?>
                <li><?= $_error ?></li>
            <? endforeach; ?>
        </ul>
        <?= $current_module->language->errors->registration->errors_found_in_form->post ?>
    </div>
<? endif; ?>

<form name="account_data_form" id="account_data_form" class="userdata_form" method="post"
      enctype="multipart/form-data" action="<?=$_SERVER["PHP_SELF"]?>">
    
    <input type="hidden" name="mode"   value="save">
    <input type="hidden" name="offset" value="<?= $_REQUEST["offset"] ?>">
    <? if($_include_account_id): ?>
        <input type="hidden" name="id_account" value="<?= $xaccount->id_account ?>">
    <? endif; ?>
    
    <? $current_module->load_extensions("edit_account_form", "top"); ?>
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->user_name ?>
        </div>
        <div class="field_input">
            <input type="text" name="user_name" readonly
                   value="<?= htmlspecialchars($xaccount->user_name) ?>">
        </div>
    </div>
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->display_name ?>
        </div>
        <div class="field_input">
            <input type="text" name="display_name" maxlength="255"
                   value="<?= htmlspecialchars($xaccount->display_name) ?>">
        </div>
    </div>
    
    <? $style = $settings->get("modules:accounts.hide_birthdate_input") == "true" ? "display: none" : ""; ?>
    <div class="cell" style="<?= $style ?>">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->birthdate ?>
        </div>
        
        <? if( ! empty($xaccount->birthdate) ): ?>
            <div class="field_input">
                <input type="text" class="aligncenter" name="birthdate" id="birthdate"
                       readonly value="<?= $xaccount->birthdate ?>">
            </div>
        <? else: ?>
            <div class="field_input">
                <input type="text" class="aligncenter" name="birthdate" id="birthdate"
                       style="width: calc(99% - 24px)"
                       value="<?= $xaccount->birthdate ?>">
                <span id="birthdate_trigger" class="fa fa-fw fa-calendar-o pseudo_link"></span>
            </div>
            <script type="text/javascript">
                $(document).ready(function()
                {
                    Calendar.setup({
                        inputField     :    "birthdate",
                        button         :    "birthdate_trigger",
                        ifFormat       :    "%Y-%m-%d",
                        align          :    "Bl",
                        singleClick    :    true
                    });
                });
            </script>
        <? endif; ?>
    </div>
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->country ?>
        </div>
        <div class="field_input">
            <select name="country">
                <option value=""><?= trim($current_module->language->user_account_form->country_select) ?></option>
                <? foreach($_country_list as $key => $val): ?>
                    <option <? if($key == $_current_user_country) echo "selected"; ?> value="<?= $key ?>"><?= $val ?></option>
                <? endforeach; ?>
            </select>
        </div>
    </div>
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->email ?>
        </div>
        <div class="field_input">
            <input type="text" name="email" maxlength="255" value="<?= htmlspecialchars($xaccount->email) ?>">
        </div>
    </div>
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->alt_email ?>
        </div>
        <div class="field_input">
            <input type="text" name="alt_email" maxlength="255" value="<?= htmlspecialchars($xaccount->alt_email) ?>">
        </div>
    </div>
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->password_change ?>
        </div>
        <div class="field_input">
            <input type="password" name="password">
        </div>
    </div>
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->password2 ?>
        </div>
        <div class="field_input">
            <input type="password" name="password2">
        </div>
    </div>
    
    <br>
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->avatar->caption ?>
        </div>
        <div class="field_input">
            <input type="hidden" name="avatar" value="<?= $xaccount->avatar ?>">
            <div class="clearfix">
                <div style="float: left; width: 48px;">
                    <a class="prettyPhoto" href="<?= $xaccount->get_avatar_url() ?>">
                        <img src="<?= $xaccount->get_avatar_url() ?>" width="48">
                    </a>
                </div>
                <div style="float: left; width: calc(100% - 48px); padding-left: 10px;">
                    <?= $current_module->language->user_account_form->avatar->upload ?><br><br>
                    <input type="file" name="uploaded_avatar"><br><br>
                    <label>
                        <? $checked = $xaccount->avatar == "@gravatar" ? "checked" : ""; ?>
                        <input type="checkbox" <?= $checked ?> name="use_gravatar" value="true">
                        <?= $current_module->language->user_account_form->avatar->use_gravatar ?>
                    </label><br><br>
                    <button type="submit">
                        <span class="fa fa-check"></span>
                        <?= $current_module->language->save_now ?>
                    </button>
                </div>
            </div>
            
        </div>
    </div>
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->banner->caption ?>
        </div>
        <div class="field_input">
            <input type="hidden" name="profile_banner" value="<?= $xaccount->profile_banner ?>">
            <a class="prettyPhoto" href="<?= $xaccount->get_profile_banner_url() ?>">
                <img src="<?= $xaccount->get_profile_banner_url() ?>" width="100%">
            </a><br>
            <?= $current_module->language->user_account_form->banner->upload ?><br><br>
            <input type="file" name="uploaded_profile_banner"><br><br>
            <button type="submit">
                <span class="fa fa-check"></span>
                <?= $current_module->language->save_now ?>
            </button>
        </div>
    </div>
    
    <div class="cell full_width">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->bio->caption ?>
        </div>
        <div class="field_input tinymce_container">
            <textarea name="bio" id="account_bio" class="tinymce"
                      placeholder="<?= $current_module->language->user_account_form->bio->placeholder ?>"><?= htmlspecialchars($xaccount->bio) ?></textarea>
        </div>
    </div>
    
    <div class="cell full_width">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->signature->caption ?>
        </div>
        <div class="field_input tinymce_container">
            <textarea name="signature" id="account_signature" class="tinymce"
                      placeholder="<?= $current_module->language->user_account_form->signature->placeholder ?>"><?= htmlspecialchars($xaccount->signature) ?></textarea>
        </div>
    </div>
    
    <div class="cell full_width">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->homepage_url->caption ?>
        </div>
        <div class="field_input">
            <input type="text" name="homepage_url" value="<?= $xaccount->homepage_url ?>"
                   placeholder="<?= $current_module->language->user_account_form->homepage_url->placeholder ?>">
        </div>
    </div>
    
    <?
    $req_level = $settings->get("modules:accounts.api_keys_access_level");
    if( empty($req_level) ) $req_level = $config::NEWCOMER_USER_LEVEL;
    if( $settings->get("modules:accounts.api_keys_support_enabled") == "true" && $xaccount->level >= $req_level ):
        $info_handler = "{$current_module->name}_profile_api_keys_info"; ?>
        
        <input type="hidden" name="deleted_api_keys" id="deleted_api_keys" value="">
        
        <style type="text/css">
            #api_keys_list .key_pair                 { display: flex; flex-wrap: wrap;
                                                       align-items: stretch; align-content: stretch;
                                                       margin: 0 0 20px 0; }
            #api_keys_list .key_pair .col            { flex-grow: 1; flex-shrink: 1; padding: 5px; }
            #api_keys_list .key_pair .input .actions { margin-top: 10px; }
            #api_keys_list .key_pair .value          { padding: 2px 4px; margin: 0; }
            #api_keys_list .key_pair .action         { white-space: nowrap; }
        </style>
        
        <section class="api_keys_container" style="text-align: left; margin-bottom: 20px;">
            
            <h2>
                <?= $current_module->language->api_keys->title ?>
                <span class="pseudo_link" onclick="toggle_info_section('<?= $info_handler ?>')"><span
                      class="fa fa-info-circle" title="<?= $language->toggle_info ?>"></span></span>
            </h2>
            
            <div class="framed_content">
                <div class="framed_content state_highlight info_handler" id="<?= $info_handler ?>"
                     style="margin-top: 0; <? if($account->engine_prefs[$info_handler] == "hidden") echo "display: none;"; ?>">
                    <span class="fa fa-times-circle pseudo_link pull-right" onclick="toggle_info_section('<?= $info_handler ?>')"></span>
                    <span class="fa fa-info-circle"></span>
                    <?= $current_module->language->api_keys->info ?>
                </div>
                
                <div id="no_api_keys_defined" class="framed_content aligncenter"
                     style="<? if( ! empty($xaccount->engine_prefs["api_keys"]) ) echo "display: none;"; ?>">
                    <br>
                    <i class="fa fa-info-circle fa-fw"></i>
                    <?= $current_module->language->api_keys->none_defined ?><br><br>
                </div>
                
                <div id="api_keys_list">
                    <?
                    if( ! empty($xaccount->engine_prefs["api_keys"]) ):
                        foreach($xaccount->engine_prefs["api_keys"] as $keypair):
                            ?>
                            <div class="framed_content key_pair">
                                
                                <input type="hidden" name="api_keys[enabled][]" value="<?= $keypair["enabled"] ?>">
                                
                                <div class="col field app_name">
                                    <div class="caption">
                                        <?= $current_module->language->api_keys->app_name ?>
                                    </div>
                                    
                                    <div class="input">
                                        <input type="text" class="value" name="api_keys[app_name][]"
                                               placeholder="<?= $current_module->language->api_keys->type_a_name ?>"
                                               value="<?= htmlspecialchars($keypair["app_name"]) ?>">
                                        <div class="actions">
                                            <span class="action enable pseudo_link" onclick="enable_keypair(this)"
                                                  style="display: none">
                                                <i class="fa fa-thumbs-o-up"></i>
                                                <?= $current_module->language->api_keys->enable ?></span>
                                            <span class="action disable pseudo_link" onclick="disable_keypair(this)">
                                                <i class="fa fa-thumbs-o-down"></i>
                                                <?= $current_module->language->api_keys->disable ?></span>
                                            <span class="action delete pseudo_link" onclick="delete_keypair(this)">
                                                <i class="fa fa-times"></i>
                                                <?= $current_module->language->api_keys->delete ?></span>
                                        </div>
                                    </div>
                                </div>
                                
                                <div class="col field public_key">
                                    <div class="caption">
                                        <?= $current_module->language->api_keys->public_key ?>
                                    </div>
                                    
                                    <div class="input">
                                        <input type="text" readonly class="value" name="api_keys[public_key][]"
                                               value="<?= htmlspecialchars($keypair["public_key"]) ?>">
                                        <div class="actions">
                                            <span class="action copy pseudo_link clipboard-copy"
                                                  data-clipboard-text="<?= htmlspecialchars($keypair["public_key"]) ?>">
                                                <i class="fa fa-copy"></i>
                                                <?= $current_module->language->api_keys->copy ?></span>
                                        </div>
                                    </div>
                                </div>
                                
                                <div class="col field secret_key">
                                    <div class="caption">
                                        <?= $current_module->language->api_keys->secret_key ?>
                                    </div>
                                    
                                    <?
                                    $secret_key = three_layer_decrypt(
                                        $keypair["secret_key"],
                                        $config->encryption_key,
                                        $xaccount->id_account,
                                        $xaccount->creation_date
                                    );
                                    ?>
                                    <div class="input">
                                        <input type="text" readonly class="value" name="api_keys[secret_key][]"
                                               value="<?= htmlspecialchars($secret_key) ?>">
                                        <div class="actions">
                                            <span class="action copy pseudo_link clipboard-copy"
                                                  data-clipboard-text="<?= htmlspecialchars($secret_key) ?>">
                                                <i class="fa fa-copy"></i>
                                                <?= $current_module->language->api_keys->copy ?></span>
                                            &nbsp;
                                            <span class="action regen pseudo_link" onclick="regenerate_secret_key(this)">
                                                <i class="fa fa-refresh"></i>
                                                <?= $current_module->language->api_keys->regenerate ?></span>
                                        </div>
                                    </div>
                                </div>
                                
                                <div class="col last_access">
                                    <div class="caption">
                                        <?= $current_module->language->api_keys->last_access ?>
                                    </div>
                                    
                                    <div class="input">
                                        <div class="value framed_content">
                                            <?
                                            $last_access = $xaccount->engine_prefs["api_access.last:{$keypair["public_key"]}"];
                                            echo empty($last_access) ? $current_module->language->api_keys->never : $last_access;
                                            ?>
                                        </div>
                                    </div>
                                </div>
                                
                            </div>
                            <?
                        endforeach;
                    endif; ?>
                </div>
                
                <p style="margin-bottom: 0;">
                    <button onclick="add_api_keypair(); return false;">
                        <i class="fa fa-plus-circle"></i>
                        <?= $current_module->language->api_keys->add ?>
                    </button>
                </p>
            </div>
            
        </section>
        
        <script type="text/javascript">
            
            function create_api_key() {
                function s4() { return Math.floor((1 + Math.random()) * 0x10000).toString(16).substring(1); }
                
                return s4() + s4() + '-' + s4() + '-' + s4() + '-' + s4() + '-' + s4() + s4() + s4();
            }
            
            function add_api_keypair()
            {
                var $pair = $( $('#api_keypair_entry').html() );
                
                var public_key = create_api_key();
                $pair.find('.col.public_key .value').val( public_key );
                $pair.find('.col.public_key .clipboard-copy').attr('data-clipboard-text', public_key);
                
                var secret_key = create_api_key().replace(/-/g, '');
                $pair.find('.col.secret_key .value').val( secret_key );
                $pair.find('.col.secret_key .clipboard-copy').attr('data-clipboard-text', secret_key);
                
                $('#api_keys_list').append( $pair );
                $('#no_api_keys_defined').hide();
            }
            
            function disable_keypair(trigger)
            {
                var message = $('#api_key_messages').find('.disable_prompt').text();
                if( ! confirm(message) ) return;
                
                var $trigger = $(trigger);
                var $pair    = $trigger.closest('.key_pair');
                
                $pair.find('input[name="api_keys[enabled][]"]').val('false');
                $pair.toggleClass('state_ok', false);
                $pair.toggleClass('state_ko', true);
                $pair.find('.action.disable').hide();
                $pair.find('.action.enable').show();
            }
            
            function enable_keypair(trigger)
            {
                var $trigger = $(trigger);
                var $pair    = $trigger.closest('.key_pair');
                
                $pair.find('input[name="api_keys[enabled][]"]').val('true');
                $pair.toggleClass('state_ok state_ko', false);
                $pair.find('.action.disable').show();
                $pair.find('.action.enable').hide();
            }
            
            function delete_keypair(trigger)
            {
                var $trigger = $(trigger);
                var $pair    = $trigger.closest('.key_pair');
                
                if( ! $pair.hasClass('state_ok') )
                {
                    var message = $('#api_key_messages').find('.delete_prompt').text();
                    if( ! confirm(message) ) return;
                    
                    var api_key  = $pair.find('.field.public_key .input .value').val();
                    var $target  = $('#deleted_api_keys');
                    
                    var actual_keys = $target.val();
                    if( actual_keys === '' ) $target.val(api_key);
                    else                     $target.val(actual_keys + ',' + api_key);
                }
                
                $pair.fadeOut('fast', function() {
                    $(this).remove();
                    if( $('#api_keys_list').find('.key_pair').length === 0 )
                        $('#no_api_keys_defined').show();
                });
            }
            
            function regenerate_secret_key(trigger)
            {
                var message = $('#api_key_messages').find('.regen_prompt').text();
                if( ! confirm(message) ) return;
                
                var $trigger = $(trigger);
                var $pair    = $trigger.closest('.key_pair');
                
                var secret_key = create_api_key().replace(/-/g, '');
                $pair.find('.col.secret_key .value').val( secret_key );
                $pair.find('.col.secret_key .clipboard-copy').attr('data-clipboard-text', secret_key);
            }
            
        </script>
        
        <div id="api_key_messages" style="display: none">
            <div class="disable_prompt"><?= unindent($current_module->language->api_keys->disable_prompt) ?></div>
            <div class="delete_prompt"><?= unindent($current_module->language->api_keys->delete_prompt) ?></div>
            <div class="regen_prompt"><?= unindent($current_module->language->api_keys->regen_prompt) ?></div>
        </div>
        
        <template id="api_keypair_entry">
            
            <div class="framed_content key_pair state_ok">
                
                <input type="hidden" name="api_keys[enabled][]" value="true">
                
                <div class="col field app_name">
                    <div class="caption">
                        <?= $current_module->language->api_keys->app_name ?>
                    </div>
                    
                    <div class="input">
                        <input type="text" class="value" name="api_keys[app_name][]"
                               placeholder="<?= $current_module->language->api_keys->type_a_name ?>">
                        <div class="actions">
                            <span class="action enable pseudo_link" onclick="enable_keypair(this)" style="display: none">
                                <i class="fa fa-thumbs-o-up"></i>
                                <?= $current_module->language->api_keys->enable ?></span>
                            <span class="action disable pseudo_link" onclick="disable_keypair(this)">
                                <i class="fa fa-thumbs-o-down"></i>
                                <?= $current_module->language->api_keys->disable ?></span>
                            <span class="action delete pseudo_link" onclick="delete_keypair(this)">
                                <i class="fa fa-times"></i>
                                <?= $current_module->language->api_keys->delete ?></span>
                        </div>
                    </div>
                </div>
                
                <div class="col field public_key">
                    <div class="caption">
                        <?= $current_module->language->api_keys->public_key ?>
                    </div>
                    
                    <div class="input">
                        <input type="text" readonly class="value" name="api_keys[public_key][]" value="N/A">
                        <div class="actions">
                            <span class="action copy pseudo_link clipboard-copy" data-clipboard-text="">
                                <i class="fa fa-copy"></i>
                                <?= $current_module->language->api_keys->copy ?></span>
                        </div>
                    </div>
                </div>
                
                <div class="col field secret_key">
                    <div class="caption">
                        <?= $current_module->language->api_keys->secret_key ?>
                    </div>
                    
                    <div class="input">
                        <input type="text" readonly class="value" name="api_keys[secret_key][]" value="N/A">
                        <div class="actions">
                            <span class="action copy pseudo_link clipboard-copy" data-clipboard-text="">
                                <i class="fa fa-copy"></i>
                                <?= $current_module->language->api_keys->copy ?></span>
                            &nbsp;
                            <span class="action regen pseudo_link" onclick="regenerate_secret_key(this)">
                                <i class="fa fa-refresh"></i>
                                <?= $current_module->language->api_keys->regenerate ?></span>
                        </div>
                    </div>
                </div>
                
                <div class="col last_access">
                    <div class="caption">
                        <?= $current_module->language->api_keys->last_access ?>
                    </div>
                    
                    <div class="input">
                        <div class="value framed_content"><?= $current_module->language->api_keys->never ?></div>
                    </div>
                </div>
                
            </div>
            
        </template>
        
        <?
    endif; ?>
    
    <? if( $config->globals["include_bottom_fields"] )
        $current_module->load_extensions("edit_account_form", "bottom"); ?>
    
    <div align="center">
        <? if( ! empty($_cancelation_redirect) ): ?>
            <button type="reset" class="big_form_submit_button"
                    onclick="location.href='<?= $_cancelation_redirect ?>'; return false;">
                <span class="fa fa-ban"></span>
                <?= $language->cancel ?>
            </button>
        <? endif; ?>
        
        <? if( ! empty($_submit_button_caption) ): ?>
            <button type="submit" class="big_form_submit_button">
                <span class="fa fa-save"></span>
                <?= $_submit_button_caption ?>
            </button>
        <? else: ?>
            <button type="submit" class="big_form_submit_button">
                <span class="fa fa-check"></span>
                <?= $language->save ?>
            </button>
        <? endif; ?>
    </div>
    
</form>
