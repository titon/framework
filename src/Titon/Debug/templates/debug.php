<div class="titon-debug">
    <?hh if (!empty($file)) { ?>
        <div class="debug-head">
            <abbr title="<?hh echo $file; ?>" class="debug-file">
                <?hh echo \Titon\Utility\Path::alias($file) . ':'; ?><!--
                --><span class="debug-line"><?hh echo $line; ?></span>
            </abbr>
        </div>
    <?hh } ?>

    <?hh foreach ($vars as $var) { ?>
        <div class="debug-output">
            <?hh if (isset($dump)) {
                echo static::_renderTemplate('table', array('value' => $var));
            } else { ?>
                <pre><code><?hh echo esc(print_r($var, true)); ?></code></pre>
            <?hh } ?>
        </div>
    <?hh } ?>
</div>