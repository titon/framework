<?php $rand = rand(0, 100); ?>

<div class="titon-backtrace">
    <table>
        <?php foreach ($backtrace as $i => $trace) { ?>
            <tr class="trace-call">
                <td class="trace-no">
                    <?= $i + 1; ?>:
                </td>
                <td class="trace-method">
                    <?php if ($trace['args']) { ?>
                        <a href="javascript:;" onclick="return toggleArgs(<?= $rand; ?>, <?= $i; ?>);" class="debug-method"><?= $trace['method']; ?></a>
                    <?php } else { ?>
                        <span class="debug-method"><?= $trace['method']; ?></span>
                    <?php } ?>
                </td>
                <td class="trace-file">
                    <abbr title="<?= $trace['file']; ?>">
                        <span class="debug-file"><?= path_alias($trace['file']); ?></span>:
                        <span class="debug-line"><?= $trace['line']; ?></span>
                    </abbr>
                </td>
            </tr>

            <?php if ($trace['args']) { ?>
                <tr id="titon-args-<?= $rand; ?>-<?= $i; ?>" class="trace-args" style="display: none">
                    <td> </td>
                    <td colspan="2">
                        <ol>
                            <?php foreach ($trace['args'] as $arg) { ?>
                                <li><?= Titon\Debug\Debugger::parseValue($arg, true); ?></li>
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

