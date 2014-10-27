<?hh // strict
/**
 * @copyright   2010-2014, The Titon Project
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
     * Return the view manager.
     *
     * @return \Titon\View\View
     */
    public function getView(): ?View;

    /**
     * Triggered before all templates are rendered at once.
     *
     * @param \Titon\Event\Event $event
     * @param \Titon\View\View $view
     * @param string $template
     */
    public function preRender(Event $event, View $view, string &$template): void;

    /**
     * Triggered after all templates are rendered at once.
     *
     * @param \Titon\Event\Event $event
     * @param \Titon\View\View $view
     * @param string $response
     */
    public function postRender(Event $event, View $view, string &$response): void;

    /**
     * Set the view manager.
     *
     * @param \Titon\View\View $view
     * @return $this
     */
    public function setView(View $view): this;

}