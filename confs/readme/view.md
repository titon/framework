# View #

A view layer provides template handling by managing lookup paths, persisting data, binding helpers,
and rendering templates through an engine. A `View` acts as a hub that manages all aspects of rendering
and piecing layouts, wrappers, partials, and templates together.

```hack
$view = new Titon\View\EngineView();
$view->addPaths(Vector {'/templates/', '/views/'});
$view->addHelper('block', new Titon\View\Helper\BlockHelper());
$view->addHelper('html', new Titon\View\Helper\HtmlHelper());
$view->setEngine(new Titon\View\Engine\TemplateEngine());

$content = $view->render('/path/to/template');
```

Helpers provide encapsulated functionality that can be used in a template.
Helpers can be accessed as variables in the template from which they were named.

```php
<?php $block->start('header'); ?>
    <h1><?= $html->anchor('Titon Framework', 'http://titon.io'); ?></h1>
<?php $block->stop(); ?>
```