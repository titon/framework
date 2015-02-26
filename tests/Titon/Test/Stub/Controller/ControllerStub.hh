<?hh // strict
namespace Titon\Test\Stub\Controller;

use Titon\Controller\AbstractController;

class ControllerStub extends AbstractController {

    public function actionWithArgs(mixed $arg1, mixed $arg2 = 0): string {
        return strval($arg1 + $arg2);
    }

    public function actionNoArgs(): string {
        return 'actionNoArgs';
    }

    public function _actionPseudoPrivate(): string {
        return 'wontBeCalled';
    }

    protected function actionProtected(): string {
        return 'wontBeCalled';
    }

    private function actionPrivate(): string {
        return 'wontBeCalled';
    }

}
