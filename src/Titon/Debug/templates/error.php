<div class="titon-debug titon-error">
    <?hh //strict
    $backtrace = static::backtrace($exception);
    $class = get_class($exception); ?>

    <div class="debug-head">
        <?hh if ($backtrace) { ?>
            <a href="javascript:;" onclick="return toggleBacktrace();" class="debug-method"><?hh echo $class; ?></a>:
        <?hh } else { ?>
            <span class="debug-method"><?hh echo $class; ?></span>:
        <?hh } ?>

        <span class="debug-message"><?hh echo esc($exception->getMessage()); ?></span>

        <abbr title="<?hh echo $exception->getFile(); ?>" class="debug-file">
            <?hh echo \Titon\Utility\Path::alias($exception->getFile()) . ':'; ?><!--
            --><span class="debug-line"><?hh echo $exception->getLine(); ?></span>
        </abbr>
    </div>

    <?hh if ($backtrace) { ?>
        <div class="debug-body" id="titon-error-backtrace" style="display: none">
            <?hh echo $backtrace; ?>
        </div>
    <?hh } ?>

    <script type="text/javascript">
        if (typeof toggleBacktrace === 'undefined') {
            function toggleBacktrace() {
                var element = document.getElementById('titon-error-backtrace');
                    element.style.display = (element.style.display === 'none' ? '' : 'none');

                return false;
            }
        }
    </script>
</div>
