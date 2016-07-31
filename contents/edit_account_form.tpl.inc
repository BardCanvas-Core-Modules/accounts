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
<script type="text/javascript">
    var tinymce_account_fileds_defaults = {};
    $(document).ready(function()
    {
        tinymce_account_fileds_defaults = $.extend({}, tinymce_defaults);
        
        tinymce_account_fileds_defaults.toolbar  = 'bold italic strikethrough forecolor removeformat | link | fullscreen';
        tinymce_account_fileds_defaults.selector = '.tinymce_account_fields';
        
        if( $_CURRENT_USER_IS_ADMIN )
            tinymce_account_fileds_defaults.toolbar = tinymce_account_fileds_defaults.toolbar + ' | code';
        
        if( $_CURRENT_USER_LANGUAGE != "en" && $_CURRENT_USER_LANGUAGE != "en_US" )
            tinymce_account_fileds_defaults.language = $_CURRENT_USER_LANGUAGE;
        
        tinymce.init(tinymce_account_fileds_defaults);
    });
</script>

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
    
    <input type="hidden" name="mode" value="save">
    <? if($_include_account_id): ?>
        <input type="hidden" name="id_account" value="<?= $xaccount->id_account ?>">
    <? endif; ?>
    
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
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->birthdate ?>
        </div>
        <div class="field_input">
            <input type="text" class="aligncenter" readonly name="birthdate" id="birthdate"
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
    </div>
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->country ?>
        </div>
        <div class="field_input">
            <select name="country">
                <option value=""><?= htmlspecialchars(trim($current_module->language->user_account_form->country_select)) ?></option>
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
                    <?= $current_module->language->user_account_form->avatar->upload ?><br>
                    <input type="file" name="uploaded_avatar"><br>
                    <label>
                        <? $checked = $xaccount->avatar == "@gravatar" ? "checked" : ""; ?>
                        <input type="checkbox" <?= $checked ?> name="use_gravatar" value="true">
                        <?= $current_module->language->user_account_form->avatar->use_gravatar ?>
                    </label>
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
            <?= $current_module->language->user_account_form->banner->upload ?><br>
            <input type="file" name="uploaded_profile_banner"><br>
        </div>
    </div>
    
    <div class="cell full_width">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->bio->caption ?>
        </div>
        <div class="field_input tinymce_container">
            <textarea name="bio" id="account_bio" class="tinymce_account_fields"
                      placeholder="<?= $current_module->language->user_account_form->bio->placeholder ?>"><?= htmlspecialchars($xaccount->bio) ?></textarea>
            <div class="tinymce_addons_bar">
                <? render_tinymce_addons_bar(); ?>
            </div>
        </div>
    </div>
    
    <div class="cell full_width">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->signature->caption ?>
        </div>
        <div class="field_input tinymce_container">
            <textarea name="signature" id="account_signature" class="tinymce_account_fields"
                      placeholder="<?= $current_module->language->user_account_form->signature->placeholder ?>"><?= htmlspecialchars($xaccount->signature) ?></textarea>
            <div class="tinymce_addons_bar">
                <? render_tinymce_addons_bar(); ?>
            </div>
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
                <span class="fa fa-check"></span>
                <?= $_submit_button_caption ?>
            </button>
        <? else: ?>
            <button type="submit" class="big_form_submit_button">
                <?= $current_module->language->user_account_form->submit ?>
                <span class="fa fa-play"></span>
            </button>
        <? endif; ?>
    </div>
    
</form>
