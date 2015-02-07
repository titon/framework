<div class="titon-<?= isset($dump) ? 'dump' : 'debug'; ?>">
    <?php if ($file) { ?>
        <div class="debug-head">
            <abbr title="<?= $file; ?>">
                <span class="debug-file"><?= path_alias($file); ?></span>:
                <span class="debug-line"><?= $line; ?></span>
            </abbr>
        </div>
    <?php } ?>

    <?php foreach ($vars as $var) { ?>
        <div class="debug-output">
            <?php if (isset($dump)) {
                echo Titon\Debug\Dumper\HtmlDumper::renderTemplate('table', ['value' => $var]);
            } else { ?>
                <pre><code><?= esc(print_r($var, true)); ?></code></pre>
            <?php } ?>
        </div>
    <?php } ?>
</div>
