<?php
/**
 * @var array             $errors
 * @var array             $messages
 * @var config            $config
 * @var \SimpleXMLElement $language
 */

use hng2_base\config;

$editable_prefs = $config->globals["editable_prefs_collection"];
$account_prefs  = $account->get_editable_prefs();
?>

<h1 class="clearfix">
    <button class="pull-right" onclick="location.href = '<?= $_SERVER["PHP_SELF"] ?>?wasuuup=<?= md5(mt_rand(1, 65535)) ?>'">
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

<form method="post" id="preferences" action="<?= $_SERVER["PHP_SELF"] ?>?wasuuup=<?=md5(mt_rand(1,65535))?>">
    
    <? foreach($editable_prefs as $title => $group_data): ?>
        
        <div class="table_wrapper">
            <table class="settings_group nav_table no_highlight">
                
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
                    ?>
                    
                    <tr class="<?= $class ?>">
                        <td width="70%">
                            <div class="principal_data">
                                <? if($specs["is_locked"] == "true") echo "<span class='fa fa-lock'></span>" ?>
                                <?= $specs->title ?>
                                <? if( $account->_is_admin): ?>
                                    <span class="framed_content inlined fixed_font" style="font-weight: normal;">
                                        <?= $key ?>
                                    </span>
                                <? endif; ?>
                            </div>
                            <blockquote>
                                <?= $specs->description ?>
                            </blockquote>
                        </td>
                        <td class="control" width="30%">
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
        <button type="submit">
            <span class="fa fa-check"></span>
            <?= $language->save ?>
        </button>
    </p>
    
</form>

<?
$generation_index = 0;
function render_pref_editor($type, $name, $value, $specs = null)
{
    global $language, $generation_index;
    
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
    
    # Default: textarea
    echo "
        <textarea class='expandible_textarea' name='engine_prefs[{$name}]'>" . htmlspecialchars($value) . "</textarea>
    ";
}
