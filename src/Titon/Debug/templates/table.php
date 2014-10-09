<?php if ($value instanceof Traversable || is_object($value)) {

    // Empty data set
    if (!$value) { ?>
        <span class="debug-type type-unknown">(empty)</span>
        <?php return;
    } ?>

    <table class="debug-table">
        <?php foreach ($value as $key => $var) { ?>

            <tr>
                <td><span class="debug-type type-object"><?php echo $key; ?></span></td>
                <td><?php echo static::parseType($var); ?></td>
                <td><?php echo static::_renderTemplate('table', ['value' => $var]); ?></td>
            </tr>

        <?php }

        // Display class methods for non-augment classes
        if (is_object($value)) {
            $methods = get_class_methods($value);
            sort($methods);

            foreach ($methods as $method) { ?>

            <tr>
                <td><span class="debug-type type-function"><?php echo $method; ?></span></td>
                <td>method</td>
                <td></td>
            </tr>

        <?php } } ?>
    </table>

<?php } else {
    echo static::parseValue($value, true);
} ?>