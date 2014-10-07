<?hh
namespace Titon\Route;

use Titon\Test\TestCase;
use Titon\Utility\Config;

/**
 * @property \Titon\Route\UrlBuilder $object
 */
class UrlBuilderTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $router = Router::registry();
        $router->map('action.ext', new TestRoute('/{module}/{controller}/{action}.{ext}', 'Module\Controller@action'));
        $router->map('action', new TestRoute('/{module}/{controller}/{action}', 'Module\Controller@action'));
        $router->map('controller', new TestRoute('/{module}/{controller}', 'Module\Controller@action'));
        $router->map('module', new TestRoute('/{module}', 'Module\Controller@action'));
        $router->map('root', new TestRoute('/', 'Module\Controller@action'));

        $this->object = UrlBuilder::registry($router);
    }

    public function testBuild() {
        $this->assertEquals('/', $this->object->build('root'));
        $this->assertEquals('/users', $this->object->build('module', Map {'module' => 'users'}));
        $this->assertEquals('/users/profile/feed.json', $this->object->build('action.ext', Map {'module' => 'users', 'controller' => 'profile', 'action' => 'feed', 'ext' => 'json'}));
    }

    public function testBuildQueryString() {
        $this->assertEquals('/users?foo=bar', $this->object->build('module', Map {'module' => 'users'}, Map {'foo' => 'bar'}));
        $this->assertEquals('/users?foo=bar&baz%5B0%5D=1&baz%5B1%5D=2&baz%5B2%5D=3', $this->object->build('module', Map {'module' => 'users'}, Map {'foo' => 'bar', 'baz' => Vector{1, 2, 3}}));
    }

    public function testBuildHashFragment() {
        $this->assertEquals('/users#foobar', $this->object->build('module', Map {'module' => 'users'}, Map {'#' => 'foobar'}));
        $this->assertEquals('/users#foo=bar', $this->object->build('module', Map {'module' => 'users'}, Map {'#' => Map {'foo' => 'bar'}}));
        $this->assertEquals('/users?key=value#foobar', $this->object->build('module', Map {'module' => 'users'}, Map {'#' => 'foobar', 'key' => 'value'}));
    }

    public function testBuildOptionalToken() {
        $this->object->getRouter()->map('blog.archives', new TestRoute('/blog/[year]/[month]/[day?]', 'Module\Controller@action'));

        $this->assertEquals('/blog/2012/2', $this->object->build('blog.archives', Map {'year' => 2012, 'month' => 02}));
        $this->assertEquals('/blog/2012/2/26', $this->object->build('blog.archives', Map {'year' => 2012, 'month' => 02, 'day' => 26}));
    }

    public function testBuildInBaseFolder() {
        $_SERVER['DOCUMENT_ROOT'] = '/root';
        $_SERVER['SCRIPT_FILENAME'] = '/root/base/index.php';

        $router = new Router();
        $router->map('module', new TestRoute('/{module}', 'Module\Controller@action'));

        $builder = new UrlBuilder($router);

        $this->assertEquals('/base', $router->base());
        $this->assertEquals('/base/users', $builder->build('module', Map {'module' => 'users'}));
    }

    public function testBuildWithLocale() {
        $route = new Route('/<locale>/{module}', 'Module\Controller@action');
        $route->addPattern('locale', Route::LOCALE)->compile();

        $this->object->getRouter()->map('locale', $route);

        $this->assertEquals('/fr-fr/forum', $this->object->build('locale', Map {'module' => 'forum', 'locale' => 'fr-fr'}));

        Config::set('titon.locale.current', 'en_US');

        $this->assertEquals('/en-us/forum', $this->object->build('locale', Map {'module' => 'forum'}));

        Config::remove('titon.locale.current');
    }

    public function testBuildTokenInflection() {
        $this->assertEquals('/download-center', $this->object->build('module', Map {'module' => 'download_center'}));
        $this->assertEquals('/customer-support', $this->object->build('module', Map {'module' => 'cusToMer-SuPPorT@#&(#'}));
        $this->assertEquals('/users/profile/feed.json', $this->object->build('action.ext', Map {'module' => 'Users', 'controller' => 'ProfILE', 'action' => 'feeD', 'ext' => 'JSON'}));
    }

    public function testBuildLinkToFunction() {
        $this->assertEquals('/users/profile/feed.json', link_to('action.ext', Map {'module' => 'users', 'controller' => 'profile', 'action' => 'feed', 'ext' => 'json'}));
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingRouteException
     */
    public function testBuildInvalidKey() {
        $this->object->build('foobar');
    }

    /**
     * @expectedException \Titon\Route\Exception\MissingTokenException
     */
    public function testBuildMissingToken() {
        $this->object->build('module');
    }

    public function testUrl() {
        $_SERVER['DOCUMENT_ROOT'] = '/root';
        $_SERVER['HTTP_HOST'] = 'sub.domain.com';
        $_SERVER['SCRIPT_FILENAME'] = '/root/base/app/index.php';
        $_SERVER['REQUEST_URI'] = '/module/controller/action.html/123?foo=bar';
        $_SERVER['HTTPS'] = 'on';
        $_GET = ['foo' => 'bar'];

        $router = new Router();

        $builder = new UrlBuilder($router);

        $this->assertEquals('https://sub.domain.com/base/app/module/controller/action.html/123?foo=bar', $builder->url());

        // url() uses the one in the registry
        $this->assertEquals('http://localhost/', url());
    }

}