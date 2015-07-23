<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\Console;

/**
 * A `Feedback` class handles the displaying of progress information to the user
 * for a specific task.
 *
 * @package Titon\Console
 */
interface Feedback {

    /**
     * Progress the feedback display.
     *
     * @return mixed
     */
    public function advance(int $increment = 1): void;

    /**
     * Force the feedback to end its output.
     *
     * @return mixed
     */
    public function finish(): void;

    /**
     * Set the frequency the feedback should update.
     *
     * @param int $interval The interval (in miliseconds)
     *
     * @return $this
     */
    public function setInterval(int $interval): this;

    /**
     * Set the message presented to the user to signify what the feedback
     * is referring to.
     *
     * @param string $message
     *
     * @return mixed
     */
    public function setMessage(string $message): this;

    /**
     * A template string used to construct additional information displayed before
     * the feedback indicator. The supported variables include message, percent,
     * elapsed, and estimated. These variables are denoted in the template '{:}'
     * notation. (i.e., '{:message} {:percent}').
     *
     * @param string $prefix
     *
     * @return mixed
     */
    public function setPrefix(string $prefix): this;

    /**
     * A template string used to construct additional information displayed after
     * the feedback indicator. The supported variables include message, percent,
     * elapsed, and estimated. These variables are denoted in the template '{:}'
     * notation. (i.e., '{:message} {:percent}').
     *
     * @param string $suffix
     *
     * @return mixed
     */
    public function setSuffix(string $suffix): this;

    /**
     * Set the total number of cycles (`advance` calls) the feedback should be
     * expected to take.
     *
     * @param int $total    The number of cycles
     *
     * @return mixed
     */
    public function setTotal(int $total): this;
}