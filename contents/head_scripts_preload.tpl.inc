
<!-- accounts :: basic :: html head include -->

<script type="text/javascript" src="<?=$config->full_root_path?>/<?= $this_module->name ?>/media/functions~v3.js"></script>
<link rel="stylesheet" type="text/css" href="<?=$config->full_root_path?>/<?= $this_module->name ?>/media/styles~v3.css">

<? if( ! $account->has_admin_rights_to_module("accounts") ) return; ?>

<!-- accounts :: admin functions -->

<script type="text/javascript" src="<?=$config->full_root_path?>/<?= $this_module->name ?>/media/admin_functions~v3.3.js"></script>
