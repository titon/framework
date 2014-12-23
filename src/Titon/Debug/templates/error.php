<div class="titon-debug titon-error">
    <?php
    $backtrace = backtrace($exception);
    $class = get_class($exception); ?>

    <div class="debug-head">
        <?php if ($backtrace) { ?>
            <a href="javascript:;" onclick="return toggleBacktrace();" class="debug-method"><?php echo $class; ?></a>:
        <?php } else { ?>
            <span class="debug-method"><?php echo $class; ?></span>:
        <?php } ?>

        <span class="debug-message"><?php echo esc($exception->getMessage()); ?></span>

        <abbr title="<?php echo $exception->getFile(); ?>" class="debug-file">
            <?php echo path_alias($exception->getFile()); ?>:<!--
            --><span class="debug-line"><?php echo $exception->getLine(); ?></span>
        </abbr>
    </div>

    <?php if ($backtrace) { ?>
        <div class="debug-body" id="titon-error-backtrace" style="display: none">
            <?php echo $backtrace; ?>
        </div>
    <?php } ?>

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
