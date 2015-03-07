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
 * The view event that occurs before the entire rendering process.
 *
 * @package Titon\View\Event
 */
class RenderingEvent extends Event {

    /**
     * The template path to render.
     *
     * @var string
     */
    protected string $template;

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
     * @param string $template
     */
    public function __construct(View $view, string $template) {
        $this->view = $view;
        $this->template = $template;

        parent::__construct('view.rendering');
    }

    /**
     * Return the template path.
     *
     * @return string
     */
    public function getTemplate(): string {
        return $this->template;
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
     * Set the template.
     *
     * @param string $template
     * @return $this
     */
    public function setTemplate(string $template): this {
        $this->template = $template;

        return $this;
    }

}
