<?php
/**
 * Editable preferences builder
 */

use hng2_cache\disk_cache;

$editable_prefs = array();
$version        = $config->module_versions_hash;
$temp_cache     = new disk_cache("{$config->datafiles_location}/cache/editable_prefs_{$version}.dat");
if( $temp_cache->loaded )
{
    $editable_prefs = $temp_cache->get_all();
    foreach($editable_prefs as $title => &$prefs)
        foreach($prefs as $key => &$spec)
            $spec = simplexml_load_string($spec);
    
    $config->globals["editable_prefs_collection"] = $editable_prefs;
    return;
}

$prefs_for_cache = array();
foreach($modules as $module_name => $module)
{
    if( empty($module->language->engine_prefs) ) continue;
    
    $title = trim($module->language->engine_prefs->title);
    /** @var \SimpleXMLElement $spec */
    foreach($module->language->engine_prefs->specs as $spec)
    {
        $key = trim($spec["key"]);
        $editable_prefs[$title]["@{$module_name}:{$key}"] = $spec;
        $prefs_for_cache[$title]["@{$module_name}:{$key}"] = $spec->asXML();
    }
}
$temp_cache->prefill($prefs_for_cache);

$config->globals["editable_prefs_collection"] = $editable_prefs;
