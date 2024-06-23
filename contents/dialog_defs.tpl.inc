
<!-- accounts :: basic :: dialog defs -->

<? if( ! $account->_exists && ! empty($_COOKIE["ip_changed"]) ): $div_id = uniqid(); ?>
    <div id="_<?= $div_id ?>" style="display:none">
        <i class="fa fa-warning"></i>
        <?= $this_module->language->ip_changed_warning ?>
    </div>
    <script type="text/javascript">
        $(document).ready(function()
        {
            throw_notification( $('#_<?= $div_id ?>').html(), 'error' );
            $.removeCookie('ip_changed', {path: '/', domain: $_COOKIES_DOMAIN})
        });
    </script>
<? endif; ?>

<? if( ! $account->_exists ): ?>
    
    <div id="login_dialog" style="display: none;" title="<?= $this_module->language->login ?>">
        <div id="login_targetarea" style="display: none;"></div>
        <form name="login_form" id="login_form" method="post" 
              action="<?=$config->full_root_path?>/accounts/scripts/login.php">
            
            <input type="hidden" name="goto"      value="<?= $_SERVER["REQUEST_URI"] ?>">
            
            <? $this_module->load_extensions("login_form", "top"); ?>
            
            <? if( empty($config->globals["@accounts:login_form.no_standard_fields"]) ): ?>
                <span class="field standard_user_name">
                    <span class="caption"><?= $this_module->language->user_name ?></span>
                    <input type="text" name="user_name">
                </span>
                <span class="field standard_password">
                    <span class="caption"><?= $this_module->language->password ?></span>
                    <span class="inline-block nowrap">
                        <input type="password" name="password" style="width: calc(100% - 36px)">
                        <span class="toggle_pass inline-block nowrap">
                            <i class="show fa fa-eye fa-fw fa-lg pseudo_link"
                               title="<?= $this_module->language->show_hide_password ?>"
                               onclick="var $this = $(this); $this.closest('.field').find('input[name=password]').attr('type', 'text'); $this.closest('.toggle_pass').find('.fa').toggle();"
                            ></i>
                            <i class="hide fa fa-eye-slash fa-fw fa-lg pseudo_link"
                               title="<?= $this_module->language->show_hide_password ?>"
                               style="display: none"
                               onclick="var $this = $(this); $this.closest('.field').find('input[name=password]').attr('type', 'password'); $this.closest('.toggle_pass').find('.fa').toggle();"
                               ></i>
                        </span>
                    </span>
                </span>
                <span class="pseudo_link standard_password_reset" onclick="$('#login_dialog').dialog('close'); reset_password();">
                    <span class="fa fa-key fa-fw"></span>
                    <?= $this_module->language->reset_password ?>
                </span>
                <? if( $settings->get("modules:accounts.register_enabled") != "false" ): ?>
                    <br>
                    <a class="standard_register" href="<?=$config->full_root_path?>/accounts/register.php">
                        <span class="fa fa-sign-in fa-fw"></span>
                        <?= $this_module->language->create_account ?>
                    </a>
                <? endif; ?>
            <? endif; ?>
            
            <? $this_module->load_extensions("login_form", "before_submit_button"); ?>
            
            <p class="aligncenter submit_button_container" style="margin-bottom: 0;">
                <button type="submit">
                    <?= $language->submit ?>
                    <span class="fa fa-play"></span>
                </button>
            </p>
            
            <? $this_module->load_extensions("login_form", "bottom"); ?>
            
        </form>
    </div>
    
    <div id="2fa_input_form"
         title="<?= $this_module->language->tfa->login_dialog->title ?>"
         data-ok-caption="<?= $language->submit ?>" data-cancel-caption="<?= $language->cancel ?>"
         style="display: none; font-family: arial, helvetica, sans-serif; font-size: 12pt; line-height: normal;">
        
        <form method="get" action="<?=$config->full_root_path?>/accounts/scripts/login.php"
              onsubmit="submit_2fa_token(); return false;">
            <p style="margin-top: 0;">
                <?= $this_module->language->tfa->login_dialog->info ?>
            </p>
            <p class="aligncenter" style="margin-bottom: 0;">
                <input type="text" id="2fa_token_input" autocomplete="off"
                       style="font-size: 18pt; text-align: center; width: 100px;"
                       maxlength="6" placeholder="000000">
            </p>
        </form>
    </div>
    
    <div id="password_reset" style="display: none;" title="<?= $this_module->language->reset_form->title ?>">
        
        <div id="reset_targetarea" style="display: none;"></div>
        
        <form name="reset_form" id="reset_form" method="post" action="<?=$config->full_root_path?>/accounts/reset_password.php">
            
            <div class="framed_content state_highlight" style="margin-top: 0;">
                <span class="fa fa-info-circle"></span>
                <span class="standard_intro">
                    <?=$this_module->language->reset_form->intro ?>
                </span>
                <? $this_module->load_extensions("reset_password_dialog", "extra_intro"); ?>
            </div>
            
            <?=$this_module->language->reset_form->your_email ?>
            <div style="margin-left: 25px;">
                <input type="text" name="email" style="width: 100%;">
            </div>
    
            <p align="right" style="margin-bottom: 0;">
                <button type="submit">
                    <?= $language->submit ?>
                    <span class="fa fa-play"></span>
                </button>
            </p>
        </form>
    </div>
    
    <div id="login_errors" style="display: none;">
        <div class="invalid_login_info"><?= $this_module->language->errors->invalid_login_info ?></div>
        <div class="ERROR_NO_EMAIL_AS_USER_NAME"><?= $this_module->language->errors->no_email_as_user_name ?></div>
        <div class="ERROR_MISSING_PARAMS"><?= $this_module->language->errors->missing_params ?></div>
        <div class="ERROR_ACCOUNT_UNEXISTENT"><?= $this_module->language->errors->account_unexistent ?></div>
        <div class="ERROR_ACCOUNT_DISABLED"><?= $this_module->language->errors->account_disabled ?></div>
        <div class="ERROR_UNCONFIRMED_ACCOUNT"><?= unindent($this_module->language->errors->verification_email_resent) ?></div>
        <div class="ERROR_WRONG_PASSWORD"><?= $this_module->language->errors->wrong_password ?></div>
        <div class="ERROR_ENGINE_DISABLED"><?= $this_module->language->errors->engine_disabled ?></div>
        <div class="ERROR_DEVICE_DISABLED"><?= $this_module->language->errors->device_disabled ?></div>
        <div class="ERROR_IP_NOT_IN_WHITELIST"><?= $this_module->language->errors->ip_not_in_whitelist ?></div>
        <div class="ERROR_INVALID_2FA_TOKEN"><?= $this_module->language->tfa->invalid_token ?></div>
        <div class="ERROR_2FA_VALIDATION_FAILED"><?= $this_module->language->tfa->verification_failed2 ?></div>
        <? $this_module->load_extensions("login", "additional_errors"); ?>
    </div>
    <div id="device_messages" style="display: none;">
        <div class="UNREGISTERED"><?= unindent($this_module->language->errors->device_unregistered) ?></div>
    </div>
    
    <div id="reset_messages" style="display: none;">
        <div class="OK"><?= $this_module->language->reset_form->OK ?></div>
    </div>
    
    <script type="text/javascript">
        $(document).ready(function() {
            if( '<?= $_GET["show_login_form"] ?>' == 'true' )
                show_login_form();
        });
    </script>
    
<? else: ?>
    
    <div id="user_menu_items" class="dropdown_menu" style="display: none;">
        
        <a class="main_menu_item <?= $current_module->name == $this_module->name && basename($_SERVER["PHP_SELF"]) == "edit_account.php" ? "current" : ""  ?>"
           href="<?= "$config->full_root_path/$this_module->name/edit_account.php" ?>">
            <span class="fa fa-edit fa-fw"></span>
            <?= $this_module->language->edit_account ?>
        </a>
        
        <? if( ! empty($config->globals["editable_prefs_collection"]) ): ?>
            <a class="main_menu_item <?= $current_module->name == $this_module->name && basename($_SERVER["PHP_SELF"]) == "preferences.php" ? "current" : ""  ?>"
               href="<?= "$config->full_root_path/$this_module->name/preferences.php" ?>">
                <span class="fa fa-list-ul fa-fw"></span>
                <?= $this_module->language->preferences ?>
            </a>
        <? endif; ?>
        
        <a class="main_menu_item <?= $current_module->name == $this_module->name && basename($_SERVER["PHP_SELF"]) == "devices.php" ? "current" : "" ?>"
           href="<?= "$config->full_root_path/$this_module->name/devices.php" ?>">
            <span class="fa fa-mobile fa-fw"></span>
            <?= $this_module->language->edit_devices ?>
        </a>
        
        <a class="main_menu_item <?= basename($_SERVER["REQUEST_URI"]) == $account->user_name ? "current" : ""  ?>"
           href="<?= "$config->full_root_path/user/{$account->user_name}" ?>">
            <span class="fa fa-home fa-fw"></span>
            <?= $this_module->language->view_my_profile ?>
        </a>
        
        <a class="main_menu_item" href="<?=$config->full_root_path?>/"
           onclick="location.href='<?=$config->full_root_path?>/accounts/scripts/logout.php'; return false;">
            <span class="fa fa-sign-out fa-fw"></span>
            <?= $this_module->language->logout ?>
        </a>
        
        <? $this_module->load_extensions("user_menu", "extra_commands"); ?>
        
    </div>
    
<? endif; ?>

<? if( ! $account->has_admin_rights_to_module("accounts") ) return; ?>

<div id="level_switch" style="display: none;">
    <select onchange="change_user_level(this)">
        <? foreach($config->user_levels_by_level as $level => $caption): ?>
            <? if( $level > $account->level ) continue; ?>
            <option value="<?= $level ?>">[<?= $level ?>] <?= $caption ?></option>
        <? endforeach; ?>
        <option disabled>──────────</option>
        <option value="!@cancel">&lt;<?= $language->cancel ?>&gt;</option>
    </select>
</div>

<div id="online_users_notification_target" style="display: none;"></div>
<div id="online_users_notification_dialog" style="display: none;"
     title="<?= $this_module->language->notify_online_users->title ?>"
     data-ok-caption="<?= $language->submit ?>" data-cancel-caption="<?= $language->cancel ?>">
    <form method="post" id="online_users_notification" action="<?= $this_module->get_url() ?>/scripts/notify_online_users.php">
        
        <p style="margin-bottom: 10px;"><?= $this_module->language->notify_online_users->select_type ?></p>
        <p style="margin-top: 0;">
            <? foreach($this_module->language->notify_online_users->types->type as $index => $type): ?>
                <label><input type="radio" <? if(trim($type["key"]) == "information") echo "checked"; ?> name="type"
                              value="<?= $type["key"] ?>"><?= $type ?></label><br>
            <? endforeach; ?>
        </p>
        
        <p style="margin-bottom: 10px;"><?= $this_module->language->notify_online_users->type_message ?></p>
        <textarea name="message" style="width: 100%" rows="4" wrap="soft"></textarea>
    </form>
</div>
