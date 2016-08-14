<?php

$editable_prefs = array();
foreach($modules as $module_name => $module)
{
    if( empty($module->language->engine_prefs) ) continue;
    
    $title = trim($module->language->engine_prefs->title);
    foreach($module->language->engine_prefs->specs as $spec)
    {
        $key = trim($spec["key"]);
        $editable_prefs[$title]["@{$module_name}:{$key}"] = $spec;
    }
}

$config->globals["editable_prefs_collection"] = $editable_prefs;
