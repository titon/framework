<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console\System;

use Titon\Console\System;

/**
 * `Abstractsystem` provides basic functionality for accessing information the
 * related to the OS application is running on.
 *
 * @package Titon\Console\System
 */
abstract class AbstractSystem implements System {

    /**
     * Boolean value if ANSI support should be forced or not.
     *
     * @var bool
     */
    protected bool $forceAnsi = false;

    /**
     * Set whether ANSI output should be forced or to only output ANSI if the
     * command line terminal supports it.
     *
     * @param bool $force   Force ANSI or not
     *
     * @return $this
     */
    public function forceAnsiSupport(bool $force): this {
        $this->forceAnsi = $force;

        return $this;
    }

    /**
     * Determines if the current command line window supports ANSI output.
     *
     * @return bool
     */
    public function hasAnsiSupport(): bool {
        if ($this->forceAnsi === true) {
            return true;
        }

        return $this->supportsAnsi();
    }

    /**
     * {@inheritdoc}
     */
    public function isPiped(): bool {
        $shellPipe = getenv('SHELL_PIPE');

        if ($shellPipe !== false) {
            return filter_var($shellPipe, FILTER_VALIDATE_BOOLEAN);
        } else {
            return function_exists('posix_isatty') && !@posix_isatty(STDOUT);
        }
    }

}
