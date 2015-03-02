<?hh // strict
/**
 * @copyright   2010-2015, The Titon Project
 * @license     http://opensource.org/licenses/bsd-license.php
 * @link        http://titon.io
 */

namespace Titon\View\Event;

use Titon\Event\Event;
use Titon\View\View;

/**
 * The view event that occurs after the entire rendering process.
 *
 * @package Titon\View\Event
 */
class RenderedEvent extends Event {

    /**
     * The rendered content.
     *
     * @var string
     */
    protected string $content;

    /**
     * The view instance.
     *
     * @var \Titon\View\View
     */
    protected View $view;

    /**
     * Store the event settings.
     *
     * @param \Titon\View\View $view
     * @param string $content
     */
    public function __construct(View $view, string $content) {
        $this->view = $view;
        $this->content = $content;

        parent::__construct('view.rendered');
    }

    /**
     * Return the content.
     *
     * @return string
     */
    public function getContent(): string {
        return $this->content;
    }

    /**
     * Return the view instance.
     *
     * @return \Titon\View\View
     */
    public function getView(): View {
        return $this->view;
    }

    /**
     * Set the content.
     *
     * @param string $content
     * @return $this
     */
    public function setContent(string $content): this {
        $this->content = $content;

        return $this;
    }

}
