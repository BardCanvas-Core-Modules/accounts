
<!-- accounts :: basic :: html head include -->

<script type="text/javascript" src="<?=$config->full_root_path?>/<?= $this_module->name ?>/media/functions~v<?=$config->scripts_version?>.js"></script>
<link rel="stylesheet" type="text/css" href="<?=$config->full_root_path?>/<?= $this_module->name ?>/media/styles~v<?=$config->scripts_version?>.css">

<? if( $this_module->name == "accounts" && $account->_is_admin ): ?>
    
    <!-- accounts :: admin functions -->
    
    <script type="text/javascript" src="<?=$config->full_root_path?>/<?= $this_module->name ?>/media/admin_functions~v<?=$config->scripts_version?>.js"></script>
    
<? endif; ?>