<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View;

use Titon\Event\Event;
use Titon\Event\Listener;

/**
 * Interface for the helpers library.
 *
 * @package Titon\View
 */
interface Helper extends Listener {

    /**
     * Return a helper that has been loaded into the view manager.
     * If no view has been set, factory the helper from registry.
     *
     * @param string $name
     * @return \Titon\View\Helper
     */
    public function getHelper(string $name): Helper;

    /**
     * Return the view manager.
     *
     * @return \Titon\View\View
     */
    public function getView(): ?View;

    /**
     * Triggered before a template (include layouts, wrappers, and partials) is rendered.
     *
     * @param \Titon\Event\Event $event
     */
    public function preRender(Event $event): void;

    /**
     * Triggered after a template is rendered.
     *
     * @param \Titon\Event\Event $event
     */
    public function postRender(Event $event): void;

    /**
     * Set the view manager.
     *
     * @param \Titon\View\View $view
     * @return $this
     */
    public function setView(View $view): this;

}
