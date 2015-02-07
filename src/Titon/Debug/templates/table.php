<?php
use Titon\Debug\Debugger;
use Titon\Debug\Dumper\HtmlDumper;

if ($value instanceof Traversable || is_object($value)) {
    if (!$value) { ?>
        <span class="debug-type type-unknown">(empty)</span>
        <?php return;
    } ?>

    <table class="debug-table">
        <?php foreach ($value as $key => $var) { ?>

            <tr>
                <td><span class="debug-type type-object"><?= $key; ?></span></td>
                <td><?= Debugger::parseType($var); ?></td>
                <td><?= HtmlDumper::renderTemplate('table', ['value' => $var]); ?></td>
            </tr>

        <?php }

        // Display class methods
        if (is_object($value)) {
            $methods = get_class_methods($value);
            sort($methods);

            foreach ($methods as $method) { ?>

            <tr>
                <td><span class="debug-type type-function"><?= $method; ?></span></td>
                <td>method</td>
                <td></td>
            </tr>

        <?php } } ?>
    </table>

<?php } else {
    echo Debugger::parseValue($value, true);
} ?>
