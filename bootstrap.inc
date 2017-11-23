<?php
/**
 * Editable preferences builder
 */

use hng2_cache\disk_cache;

$editable_prefs = array();
$version        = $config->module_versions_hash;
$cache_file     = "{$config->datafiles_location}/cache/editable_prefs_{$version}-{$settings->get("engine.default_language")}.dat";
$temp_cache     = new disk_cache($cache_file);
if( $temp_cache->loaded )
{
    $incoming = $temp_cache->get_all();
    foreach($incoming as $title => $prefs)
        foreach($prefs as $key => $spec)
            $editable_prefs[$title][$key] = simplexml_load_string($spec);
    
    $config->globals["editable_prefs_collection"] = $editable_prefs;
    return;
}

$prefs_for_cache = array();

if( ! empty($language->engine_prefs) )
{
    $title = trim($language->engine_prefs->title);
    /** @var \SimpleXMLElement $spec */
    foreach($language->engine_prefs->specs as $spec)
    {
        $key = trim($spec["key"]);
        $editable_prefs[$title]["!core:{$key}"] = $spec;
        $prefs_for_cache[$title]["!core:{$key}"] = $spec->asXML();
    }
}

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
