<div class="titon-inspect">
    <?php
    $backtrace = backtrace($exception);
    $class = get_class($exception); ?>

    <div class="debug-head">
        <?php if ($backtrace) { ?>
            <a href="javascript:;" onclick="return toggleBacktrace();" class="debug-method"><?= $class; ?></a>:
        <?php } else { ?>
            <span class="debug-method"><?= $class; ?></span>:
        <?php } ?>

        <span class="debug-message"><?= esc($exception->getMessage()); ?></span>

        <abbr title="<?= $exception->getFile(); ?>">
            <span class="debug-file"><?= path_alias($exception->getFile()); ?></span>:
            <span class="debug-line"><?= $exception->getLine(); ?></span>
        </abbr>
    </div>

    <?php if ($backtrace) { ?>
        <div class="debug-body" id="titon-inspect-backtrace" style="display: none">
            <?= $backtrace; ?>
        </div>
    <?php } ?>

    <script type="text/javascript">
        if (typeof toggleBacktrace === 'undefined') {
            function toggleBacktrace() {
                var element = document.getElementById('titon-inspect-backtrace');
                    element.style.display = (element.style.display === 'none' ? '' : 'none');

                return false;
            }
        }
    </script>
</div>
