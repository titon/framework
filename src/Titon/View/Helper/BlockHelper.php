<?hh //strict
/**
 * @copyright   2010-2014, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Helper;

use Titon\View\Exception\ActiveBlockException;

/**
 * The BlockHelper can be used to capture output within the view layer and render at a later time.
 * Blocks can be created by triggering a start and stop, and all contents in between will be captured.
 *
 * @package Titon\View\Helper
 */
class BlockHelper extends AbstractHelper {

    /**
     * List of unclosed blocks.
     *
     * @type array
     */
    protected $_active = [];

    /**
     * List of captured output.
     *
     * @type array
     */
    protected $_blocks = [];

    /**
     * Return the last opened block in the active list.
     *
     * @return string
     */
    public function active() {
        return end($this->_active);
    }

    /**
     * Append a string to a block.
     * If a block does not exist, create it.
     *
     * @param string $key
     * @param string $value
     * @return $this
     */
    public function append($key, $value) {
        if (!$this->has($key)) {
            $this->set($key, '');
        }

        $this->_blocks[$key] .= (string) $value;

        return $this;
    }

    /**
     * Return the contents of a block.
     * If no block is found, return a default value.
     *
     * @param string $key
     * @param string $default
     * @return string
     */
    public function get($key, $default = null) {
        if ($this->has($key)) {
            return $this->_blocks[$key];
        }

        return $default;
    }

    /**
     * Check if a block exists.
     *
     * @param string $key
     * @return bool
     */
    public function has($key) {
        return isset($this->_blocks[$key]);
    }

    /**
     * Return the current contents for the active block.
     * This should be nested within a block.
     *
     * @return string
     */
    public function parent() {
        return $this->get($this->active());
    }

    /**
     * Prepend a string to a block.
     * If a block does not exist, create it.
     *
     * @param string $key
     * @param string $value
     * @return $this
     */
    public function prepend($key, $value) {
        if (!$this->has($key)) {
            $this->set($key, '');
        }

        $this->set($key, (string) $value . $this->_blocks[$key]);

        return $this;
    }

    /**
     * Directly set the contents of a block.
     * This will overwrite any previous contents.
     *
     * @param string $key
     * @param mixed $value
     * @return $this
     */
    public function set($key, $value) {
        $this->_blocks[$key] = $value;

        return $this;
    }

    /**
     * Stop the currently active block and return its contents.
     *
     * @return string
     */
    public function show() {
        $active = $this->active();

        $this->stop();

        return $this->get($active);
    }

    /**
     * Start capturing output for a block defined by key.
     * An active block must be stopped to capture any output.
     *
     * @param string $key
     * @return $this
     * @throws \Titon\View\Exception\ActiveBlockException
     */
    public function start($key) {
        if (in_array($key, $this->_active)) {
            throw new ActiveBlockException(sprintf('A %s block is already active, please stop one before continuing', $key));
        }

        $this->_active[] = $key;
        ob_start();

        return $this;
    }

    /**
     * Stop capturing a block and save the contents.
     *
     * @return $this
     */
    public function stop() {
        if (empty($this->_active)) {
            return $this;
        }

        $this->set($this->active(), ob_get_clean());
        array_pop($this->_active);

        return $this;
    }

}