<?hh
namespace Titon\Intl;

use Titon\Io\Reader\HackReader;
use Titon\Route\Router;
use Titon\Route\UrlBuilder;
use Titon\Test\TestCase;
use Titon\Test\Stub\Intl\RouteResolverStub;

/**
 * @property \Titon\Intl\RouteResolver $object
 */
class RouteResolverTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->translator = new Translator(new MessageLoader(Vector {new HackReader()}));
        $this->translator->addLocale(new Locale('en_US'));
        $this->translator->addLocale(new Locale('fr'));
        $this->translator->setFallback('en');

        $this->builder = new UrlBuilder(new Router());

        $this->object = new RouteResolverStub($this->translator, $this->builder);
    }

    public function testNoLocaleRedirectsToFallback(): void {
        ob_start();
        $this->object->resolve('/foo');
        $redirect = ob_get_clean();

        $this->assertEquals('/en/foo', $redirect);
    }

    public function testUnsupportedLocaleRedirectsToFallback(): void {
        ob_start();
        $this->object->resolve('/es');
        $redirect = ob_get_clean();

        $this->assertEquals('/en', $redirect);
    }

    public function testUnsupportedLocaleWithPathRedirectsToFallback(): void {
        ob_start();
        $this->object->resolve('/es/foo');
        $redirect = ob_get_clean();

        $this->assertEquals('/en/foo', $redirect);
    }

    public function testSupportedLocaleResolves(): void {
        ob_start();
        $this->object->resolve('/en/foo');
        $redirect = ob_get_clean();

        $this->assertEquals('', $redirect);
    }

    public function testSupportedLocaleDoubleCodeResolves(): void {
        ob_start();
        $this->object->resolve('/en-us/foo');
        $redirect = ob_get_clean();

        $this->assertEquals('', $redirect);
    }

    public function testLocaleOnlyResolves(): void {
        ob_start();
        $this->object->resolve('/en');
        $redirect = ob_get_clean();

        $this->assertEquals('', $redirect);
    }

    public function testLocaleDoubleCodeOnlyResolves(): void {
        ob_start();
        $this->object->resolve('/en-us');
        $redirect = ob_get_clean();

        $this->assertEquals('', $redirect);
    }

}
