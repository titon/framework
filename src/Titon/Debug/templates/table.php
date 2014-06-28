<?hh if (is_array($value) || is_object($value)) {

    // Empty data set
    if (empty($value)) { ?>
        <span class="debug-type unknown">(empty)</span>
        <?hh return;
    } ?>

    <table class="debug-table">
        <?hh foreach ($value as $key => $var) { ?>

            <tr>
                <td><span class="debug-type object"><?hh echo $key; ?></span></td>
                <td><?hh echo static::parseType($var); ?></td>
                <td><?hh echo static::_renderTemplate('table', array('value' => $var)); ?></td>
            </tr>

        <?hh }

        // Display class methods for non-augment classes
        if (is_object($value) && strpos(get_class($value), 'Augment') === false) {
            $methods = get_class_methods($value);
            sort($methods);

            foreach ($methods as $method) { ?>

            <tr>
                <td><span class="debug-type function"><?hh echo $method; ?></span></td>
                <td>method</td>
                <td></td>
            </tr>

        <?hh } } ?>
    </table>

<?hh } else {
    echo static::parseValue($value, true);
} ?>