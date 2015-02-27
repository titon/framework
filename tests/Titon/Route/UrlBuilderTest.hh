<?hh
namespace Titon\Route;

use Titon\Context\Depository;
use Titon\Test\Stub\Route\TestRouteStub;
use Titon\Test\TestCase;
use Titon\Utility\Config;
use Titon\Utility\State\Get;
use Titon\Utility\State\Server;

/**
 * @property \Titon\Route\UrlBuilder $object
 */
class UrlBuilderTest extends TestCase {

    public static function setUpBeforeClass(): void {
        $container = Depository::getInstance();
        $container->singleton('Titon\Route\Router');
        $container->register('Titon\Route\UrlBuilder')->with($container->make('Titon\Route\Router'));
    }

    public static function tearDownAfterClass(): void {
        Depository::getInstance()->clear();
    }

    protected function setUp(): void {
        parent::setUp();

        $container = Depository::getInstance();

        $router = $container->make('Titon\Route\Router');

        invariant($router instanceof Router, 'Must be a Router.');

        $router->map('action.ext', new TestRouteStub('/{module}/{controller}/{action}.{ext}', 'Module\Controller@action'));
        $router->map('action', new TestRouteStub('/{module}/{controller}/{action}', 'Module\Controller@action'));
        $router->map('controller', new TestRouteStub('/{module}/{controller}', 'Module\Controller@action'));
        $router->map('module', new TestRouteStub('/{module}', 'Module\Controller@action'));
        $router->map('root', new TestRouteStub('/', 'Module\Controller@action'));

        $this->object = $builder = $container->make('Titon\Route\UrlBuilder');

        invariant($builder instanceof UrlBuilder, 'Must be a UrlBuilder.');
    }

    public function testBuild(): void {
        $this->assertEquals('/', $this->object->build('root'));
        $this->assertEquals('/users', $this->object->build('module', Map {'module' => 'users'}));
        $this->assertEquals('/users/profile/feed.json', $this->object->build('action.ext', Map {'module' => 'users', 'controller' => 'profile', 'action' => 'feed', 'ext' => 'json'}));
    }

    public function testBuildQueryString(): void {
        $this->assertEquals('/users?foo=bar', $this->object->build('module', Map {'module' => 'users'}, Map {'foo' => 'bar'}));
        $this->assertEquals('/users?foo=bar&baz%5B0%5D=1&baz%5B1%5D=2&baz%5B2%5D=3', $this->object->build('module', Map {'module' => 'users'}, Map {'foo' => 'bar', 'baz' => Vector{1, 2, 3}}));
    }

    public function testBuildHashFragment(): void {
        $this->assertEquals('/users#foobar', $this->object->build('module', Map {'module' => 'users'}, Map {'#' => 'foobar'}));
        $this->assertEquals('/users#foo=bar', $this->object->build('module', Map {'module' => 'users'}, Map {'#' => Map {'foo' => 'bar'}}));
        $this->assertEquals('/users?key=value#foobar', $this->object->build('module', Map {'module' => 'users'}, Map {'#' => 'foobar', 'key' => 'value'}));
    }

    public function testBuildOptionalToken(): void {
        $this->object->getRouter()->map('blog.archives', new TestRouteStub('/blog/[year]/[month]/[day?]', 'Module\Controller@action'));

        $this->assertEquals('/blog/2012/2', $this->object->build('blog.archives', Map {'year' => 2012, 'month' => 02}));
        $this->assertEquals('/blog/2012/2/26', $this->object->build('blog.archives', Map {'year' => 2012, 'month' => 02, 'day' => 26}));
    }

    public function testBuildInBaseFolder(): void {
        $_SERVER['DOCUMENT_ROOT'] = '/root';
        $_SERVER['SCRIPT_FILENAME'] = '/root/base/index.php';

        Server::initialize($_SERVER);

        $router = new Router();
        $router->map('module', new TestRouteStub('/{module}', 'Module\Controller@action'));

        $builder = new UrlBuilder($router);

        $this->assertEquals('/base', $router->base());
        $this->assertEquals('/base/users', $builder->build('module', Map {'module' => 'users'}));
    }

    public function testBuildWithLocale(): void {
        $route = new Route('/<locale>/{module}', 'Module\Controller@action');
        $route->addPattern('locale', Route::LOCALE)->compile();

        $this->object->getRouter()->map('locale', $route);

        $this->assertEquals('/fr-fr/forum', $this->object->build('locale', Map {'module' => 'forum', 'locale' => 'fr-fr'}));

        Config::set('titon.locale.current', 'en_US');

        $this->assertEquals('/en-us/forum', $this->object->build('locale', Map {'module' => 'forum'}));

        Config::remove('titon.locale.current');
    }

    public function testBuildTokenInflection(): void {
        $this->assertEquals('/download-center', $this->object->build('module', Map {'module' => 'download_center'}));
        $this->assertEquals('/customer-support', $this->object->build('module', Map {'module' => 'cusToMer-SuPPorT@#&(#'}));
        $this->assertEquals('/users/profile/feed.json', $this->object->build('action.ext', Map {'module' => 'Users', 'controller' => 'ProfILE', 'action' => 'feeD', 'ext' => 'JSON'}));
    }

    public function testBuildLinkToFunction(): void {
        $this->assertEquals('/users/profile/feed.json', link_to('action.ext', Map {'module' => 'users', 'controller' => 'profile', 'action' => 'feed', 'ext' => 'json'}));
    }

    public function testBuildCaching(): void {
        $this->assertFalse($this->object->hasCache('Titon\Route\UrlBuilder::build-module-97a181b0b36bd36f504742ea2acd6746'));

        $this->assertEquals('/users', $this->object->build('module', Map {'module' => 'users'}));

        $this->assertTrue($this->object->hasCache('Titon\Route\UrlBuilder::build-module-97a181b0b36bd36f504742ea2acd6746'));

        $this->assertEquals('/users?foo=bar', $this->object->build('module', Map {'module' => 'users'}, Map {'foo' => 'bar'}));
        $this->assertEquals('/users?foo=baz', $this->object->build('module', Map {'module' => 'users'}, Map {'foo' => 'baz'}));
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingRouteException
     */
    public function testBuildInvalidKey(): void {
        $this->object->build('foobar');
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingTokenException
     */
    public function testBuildMissingToken(): void {
        $this->object->build('module');
    }

    public function testUrl(): void {
        $_SERVER['DOCUMENT_ROOT'] = '/root';
        $_SERVER['HTTP_HOST'] = 'sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/root/base/app/index.php';
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html/123?foo=bar';
        $_SERVER['HTTPS'] = 'on';
        Server::initialize($_SERVER);

        $_GET = ['foo' => 'bar'];
        Get::initialize($_GET);

        $router = new Router();

        $builder = new UrlBuilder($router);

        $this->assertEquals('https://sub.domain.com/base/app/module/controller/action.html/123?foo=bar', $builder->url());

        // url() uses the one in the registry
        $this->assertEquals('http://localhost/', url());
    }

}
