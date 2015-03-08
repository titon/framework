<?hh // strict
namespace Titon\Test\Stub\View;

use Titon\View\DataMap;
use Titon\View\AbstractView;

class ViewStub extends AbstractView {

    public function render(string $template, bool $private = false): string {
        return '';
    }

    public function renderTemplate(string $path, DataMap $variables = Map {}): string {
        return '';
    }

}
