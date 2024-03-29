<?php
/**
 * @var module            $current_module
 * @var array             $_errors
 * @var array             $_country_list
 * @var string            $_current_user_country
 * @var string            $_cancelation_redirect
 * @var string            $_submit_button_caption
 * @var string            $_form_title
 * @var bool              $_hide_captcha
 * @var bool              $_no_flag_check
 * @var \SimpleXMLElement $language
 * @var settings          $settings
 * @var config            $config
 */

use hng2_base\config;
use hng2_base\module;
use hng2_base\settings;
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
    
    <?= empty($_form_title) ? $current_module->language->register_form->form_title : $_form_title ?>
</h1>

<? if( ! $_no_flag_check ): ?>
    <? if( $settings->get("modules:accounts.register_enabled") != "true" ): ?>
        
        <div class="framed_content state_ok">
            <span class="fa fa-info-circle"></span>
            <?= $current_module->language->register_form->registering_disabled ?>
        </div>
        <? return; ?>
        
    <? endif; ?>
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

<?
if( $_POST["mode"] != "create" )
{
    $custom_banner = $settings->get("modules:accounts.custom_registration_banner");
    if( ! $_no_flag_check && ! empty($custom_banner) ) echo "<div class='custom_registration_baner'>$custom_banner</div>";
}
?>

<? if( ! $_hide_infos ): ?>
    <div class="framed_content state_ok">
        <span class="fa fa-info-circle"></span>
        <?= $current_module->language->register_form->form_info ?>
    </div>
<? endif; ?>

<form name="register_form" id="register_form" class="userdata_form" method="post" action="<?=$_SERVER["PHP_SELF"]?>" onsubmit="return validate_register_form()">
    
    <input type="hidden" name="mode"      value="create">
    <input type="hidden" name="redir_url" value="<?= empty($_REQUEST["redir_url"]) ? "" : $_REQUEST["redir_url"] ?>">
    
    <? $current_module->load_extensions("register_form", "top"); ?>
    
    <? if( $settings->get("modules:accounts.automatic_user_names") != "true" || $_no_flag_check ): ?>
        <div class="cell">
            <div class="field_caption">
                <?= $current_module->language->user_account_form->user_name ?>
            </div>
            <div class="field_input">
                <? $placeholder = $account->has_admin_rights_to_module("accounts")
                    ? $current_module->language->user_account_form->user_name_placeholder
                    : ""; ?>
                <input type="text" name="user_name" maxlength="64" value="<?= htmlspecialchars($xaccount->user_name) ?>"
                       placeholder="<?= $placeholder ?>">
                <? if( ! $_hide_infos ): ?>
                    <div class="framed_content state_ok">
                        <span class="fa fa-info-circle"></span>
                        <?= $current_module->language->user_account_form->user_name_info ?>
                    </div>
                <? endif; ?>
            </div>
        </div>
    <? endif; ?>
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->display_name ?>
        </div>
        <div class="field_input">
            <input type="text" name="display_name" maxlength="255" value="<?= htmlspecialchars($xaccount->display_name) ?>">
            <? if( ! $_hide_infos ): ?>
                <div class="framed_content state_ok">
                    <span class="fa fa-info-circle"></span>
                    <?= $settings->get("modules:accounts.automatic_user_names") == "true"
                        ? $current_module->language->register_form->automatic_user_names_info
                        : $current_module->language->user_account_form->display_name_info ?>
                </div>
            <? endif; ?>
        </div>
    </div>
    
    <? $style = $settings->get("modules:accounts.hide_birthdate_input") == "true" ? "display: none" : ""; ?>
    <div class="cell" style="<?= $style ?>">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->birthdate ?>
        </div>
        <div class="field_input">
            <? $placeholder = $account->has_admin_rights_to_module("accounts")
                ? $current_module->language->user_account_form->admin_ommitable
                : ""; ?>
            <input type="text" class="aligncenter" name="birthdate" id="birthdate"
                   style="width: calc(99% - 24px)" placeholder="<?= $placeholder ?>"
                   value="<?= $xaccount->birthdate ?>">
            <span id="birthdate_trigger" class="fa fa-fw fa-calendar-o pseudo_link"></span>
            <? if( ! $_hide_infos ): ?>
                <div class="framed_content state_ok">
                    <span class="fa fa-info-circle"></span>
                    <?= $current_module->language->user_account_form->brithdate_info ?>
                </div>
            <? endif; ?>
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
    
    <? if( $settings->get("modules:accounts.non_mandatory_country") != "true" || $_no_flag_check ): ?>
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
    <? endif; ?>
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->email ?>
        </div>
        <div class="field_input">
            <input type="text" name="email" maxlength="255" value="<?= htmlspecialchars($xaccount->email) ?>">
            <? if( ! $_hide_infos ): ?>
                <div class="framed_content state_ok">
                    <span class="fa fa-info-circle"></span>
                    <?= $current_module->language->user_account_form->email_message ?>
                </div>
            <? endif; ?>
        </div>
    </div>
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->alt_email ?>
        </div>
        <div class="field_input">
            <? $placeholder = $account->has_admin_rights_to_module("accounts")
                ? $current_module->language->user_account_form->admin_ommitable
                : ""; ?>
            <input type="text" name="alt_email" maxlength="255" value="<?= htmlspecialchars($xaccount->alt_email) ?>"
                   placeholder="<?= $placeholder ?>">
            <? if( ! $_hide_infos ): ?>
                <div class="framed_content state_ok">
                    <span class="fa fa-info-circle"></span>
                    <?= $current_module->language->user_account_form->alt_email_message ?>
                </div>
            <? endif; ?>
        </div>
    </div>
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->password ?>
            <? if( $account->has_admin_rights_to_module("accounts"))
                echo "
                    <span class='pseudo_link' onclick=\"prompt( '{$current_module->language->user_account_form->copy_random_password}', Math.random().toString(36).slice(2).substr(1, 12) )\">
                        <i class='fa fa-cog fa-fw'></i>{$current_module->language->user_account_form->set_random_password}
                    </span>
                "; ?>
        </div>
        <div class="field_input">
            <input type="password" name="password" value="<?= htmlspecialchars(stripslashes($_POST["password"])) ?>">
        </div>
    </div>
    
    <div class="cell">
        <div class="field_caption">
            <?= $current_module->language->user_account_form->password2 ?>
        </div>
        <div class="field_input">
            <input type="password" name="password2" value="<?= htmlspecialchars(stripslashes($_POST["password2"])) ?>">
        </div>
    </div>
    
    <? if( ! $_hide_infos ): ?>
        <div class="framed_content state_ok">
            <span class="fa fa-info-circle"></span>
            <?= $current_module->language->user_account_form->password_message ?>
        </div>
    <? endif; ?>
    
    <?
    $current_module->load_extensions("register_form", "before_bottom");
    
    if( $config->globals["include_bottom_fields"] )
        $current_module->load_extensions("register_form", "bottom"); ?>
    
    <div align="center">
        
        <? if( $settings->get("engine.recaptcha_public_key") != "" && ! $_hide_captcha ): ?>
            <script src='https://www.google.com/recaptcha/api.js'></script>
            <div class="g-recaptcha" data-sitekey="<?= $settings->get("engine.recaptcha_public_key") ?>"></div>
            <br>
        <? endif; ?>
        
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
                <?= $current_module->language->user_account_form->submit ?>
                <span class="fa fa-play"></span>
            </button>
        <? endif; ?>
    </div>
    
</form>

<div id="register_form_errors" style="display: none;">
    <span class="display_name"><?= $current_module->language->errors->registration->missing->display_name ?></span>
    <span class="user_name"><?= $current_module->language->errors->registration->missing->user_name ?></span>
    <span class="country"><?= $current_module->language->errors->registration->missing->country ?></span>
    <span class="email"><?= $current_module->language->errors->registration->missing->email ?></span>
    <span class="password"><?= $current_module->language->errors->registration->missing->password ?></span>
    <span class="password2"><?= $current_module->language->errors->registration->missing->password2 ?></span>
    <span class="recaptcha_response_field"><?= $current_module->language->errors->registration->missing->recaptcha_response_field ?></span>
    <span class="passwords_mismatch"><?= $current_module->language->errors->registration->invalid->passwords_mismatch ?></span>
    <span class="mails_must_be_different"><?= $current_module->language->errors->registration->invalid->mails_must_be_different ?></span>
</div>
