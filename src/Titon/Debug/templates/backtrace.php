<div class="titon-debug titon-backtrace">
    <table>
        <?hh $rand = rand(0, 100);

        foreach ($backtrace as $i => $trace) { ?>

            <tr class="trace-call">
                <td class="cell-no">
                    <?hh echo $i + 1; ?>:
                </td>
                <td class="cell-method">
                    <?hh if (!empty($trace['args'])) { ?>
                        <a href="javascript:;" onclick="return toggleArgs(<?hh echo $rand; ?>, <?hh echo $i; ?>);" class="debug-method"><?hh echo $trace['method']; ?></a>
                    <?hh } else { ?>
                        <span class="debug-method"><?hh echo $trace['method']; ?></span>
                    <?hh } ?>
                </td>
                <td class="cell-file">
                    <abbr title="<?hh echo $trace['file']; ?>" class="debug-file">
                        <?hh echo \Titon\Utility\Path::alias($trace['file']) . ':'; ?><!--
                        --><span class="debug-line"><?hh echo $trace['line']; ?></span>
                    </abbr>
                </td>
            </tr>

            <?hh if (!empty($trace['args'])) { ?>

                <tr id="titon-args-<?hh echo $rand; ?>-<?hh echo $i; ?>" class="trace-args" style="display: none">
                    <td> </td>
                    <td colspan="2">
                        <ol>
                            <?hh foreach ($trace['args'] as $arg) { ?>
                                <li><?hh echo static::parseValue($arg, true); ?></li>
                            <?hh } ?>
                        </ol>
                    </td>
                </tr>

            <?hh }
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

