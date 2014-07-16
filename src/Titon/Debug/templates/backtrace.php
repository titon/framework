<div class="titon-debug titon-backtrace">
    <table>
        <?php $rand = rand(0, 100);

        foreach ($backtrace as $i => $trace) { ?>

            <tr class="trace-call">
                <td class="cell-no">
                    <?php echo $i + 1; ?>:
                </td>
                <td class="cell-method">
                    <?php if (!empty($trace['args'])) { ?>
                        <a href="javascript:;" onclick="return toggleArgs(<?php echo $rand; ?>, <?php echo $i; ?>);" class="debug-method"><?php echo $trace['method']; ?></a>
                    <?php } else { ?>
                        <span class="debug-method"><?php echo $trace['method']; ?></span>
                    <?php } ?>
                </td>
                <td class="cell-file">
                    <abbr title="<?php echo $trace['file']; ?>" class="debug-file">
                        <?php echo path_alias($trace['file']); ?>:<!--
                        --><span class="debug-line"><?php echo $trace['line']; ?></span>
                    </abbr>
                </td>
            </tr>

            <?php if (!empty($trace['args'])) { ?>

                <tr id="titon-args-<?php echo $rand; ?>-<?php echo $i; ?>" class="trace-args" style="display: none">
                    <td> </td>
                    <td colspan="2">
                        <ol>
                            <?php foreach ($trace['args'] as $arg) { ?>
                                <li><?php echo static::parseValue($arg, true); ?></li>
                            <?php } ?>
                        </ol>
                    </td>
                </tr>

            <?php }
        } ?>
    </table>

    <script type="text/javascript">
        if (typeof toggleArgs === 'undefined') {
            function toggleArgs(rand, id) {
                var element = document.getElementById('titon-args-' + rand + '-' + id);
                    element.style.display = (element.style.display === 'none' ? '' : 'none');

                return false;
            }
        }
    </script>
</div>

