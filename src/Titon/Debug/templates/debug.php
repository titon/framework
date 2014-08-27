<div class="titon-debug">
    <?php if (isset($file)) { ?>
        <div class="debug-head">
            <abbr title="<?php echo $file; ?>" class="debug-file">
                <?php echo path_alias($file) . ':'; ?><!--
                --><span class="debug-line"><?php echo $line; ?></span>
            </abbr>
        </div>
    <?php } ?>

    <?php foreach ($vars as $var) { ?>
        <div class="debug-output">
            <?php if (isset($dump)) {
                echo static::_renderTemplate('table', array('value' => $var));
            } else { ?>
                <pre><code><?php echo esc(print_r($var, true)); ?></code></pre>
            <?php } ?>
        </div>
    <?php } ?>
</div>