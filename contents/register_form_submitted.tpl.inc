
<h1><?= $current_module->language->register_form->form_submitted->title ?></h1>

<?= $current_module->language->register_form->form_submitted->content ?>

<? $current_module->load_extensions("register_form", "after_successful_creation"); ?>
