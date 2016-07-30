
function toggle_registration_mode(new_mode)
{
    var url = $_FULL_ROOT_PATH + '/accounts/scripts/toolbox.php';
    var params = {
        mode: new_mode == 'on' ? 'enable_register' : 'disable_register',
        wasuuup: parseInt(Math.random() * 1000000000000000)
    };
    
    $.blockUI(blockUI_default_params);
    $.get(url, params, function(response)
    {
        if( response != 'OK' )
        {
            alert( response );
            $.unblockUI();
            
            return;
        }
        
        location.href = $_PHP_SELF + '?wasuuup=' + parseInt(Math.random() * 1000000000000000);
    });
}

function switch_admin(id_account, admin_action)
{
    var url = $_FULL_ROOT_PATH + '/accounts/scripts/toolbox.php';
    var params = {
        id_account: id_account,
        mode:       admin_action,
        wasuuup:    parseInt(Math.random() * 1000000000000000)
    };
    
    var $tr = $('#accounts_nav').find('tr[id_account="' + id_account + '"]');
    $tr.block(blockUI_smallest_params);
    $.get(url, params, function(response)
    {
        if( response != 'OK' )
        {
            alert( response );
            $tr.unblock();
            
            return;
        }
    
        location.href = $_PHP_SELF + '?wasuuup=' + parseInt(Math.random() * 1000000000000000);
    });
}

function toggle_account(id_account, new_mode)
{
    var url = $_FULL_ROOT_PATH + '/accounts/scripts/toolbox.php';
    var params = {
        id_account: id_account,
        mode:       new_mode,
        wasuuup:    parseInt(Math.random() * 1000000000000000)
    };
    
    var $tr = $('#accounts_nav').find('tr[id_account="' + id_account + '"]');
    $tr.block(blockUI_smallest_params);
    $.get(url, params, function(response)
    {
        if( response != 'OK' )
        {
            alert( response );
            $tr.unblock();
            
            return;
        }
    
        location.href = $_PHP_SELF + '?wasuuup=' + parseInt(Math.random() * 1000000000000000);
    });
}
