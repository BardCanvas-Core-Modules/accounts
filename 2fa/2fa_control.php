<?php
/**
 * 2fa control for the settings page
 *
 * @package    HNG2
 * @subpackage accounts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

include "../../config.php";
include "../../includes/bootstrap.inc";

header("Content-Type: text/html; chrset=utf-8");

if( ! $account->_exists ) throw_fake_401();
if( $account->state != "enabled" ) throw_fake_401();

$secret = $account->engine_prefs["@accounts:2fa_secret"];
?>
<div style="white-space: normal" id="2fa_settings">
    
    <?if( empty($secret) ): ?>
        
        <p>
            <i class="fa fa-info-circle"></i>
            <?= $current_module->language->tfa->not_configured ?>
        </p>
        <p>
            <span class="framed_content inlined state_ok pseudo_link" onclick="open_2fa_configuration_dialog()">
                <i class="fa fa-play"></i>
                <?= $current_module->language->tfa->enable ?>
            </span>
        </p>
        
    <? else: ?>
        
        <p>
            <i class="fa fa-info-circle"></i>
            <?= $current_module->language->tfa->configured ?>
        </p>
        <p>
            <input type="text" id="tfa_disabling_token_input" autocomplete="off"
                   style="width: 70px; text-align: center; font-size: 12pt;"
                   maxlength="6" placeholder="000000">
            <span class="framed_content inlined state_ok pseudo_link"
                  onclick="disable_2fa()">
                <i class="fa fa-play"></i>
                <?= $current_module->language->tfa->disable ?>
            </span>
        </p>
        
    <? endif; ?>
</div>
