<?hh // strict
namespace Titon\Test\Stub\View;

use Titon\View\DataMap;
use Titon\View\Engine\AbstractEngine;

class EngineStub extends AbstractEngine {

    public function open(string $partial, DataMap $variables = Map {}): string {
        return '';
    }

    public function render(string $path, DataMap $variables = Map {}): string {
        return '';
    }

    public function getExtension(): string {
        return '';
    }

}
