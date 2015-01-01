# Route #

Provides a mechanism for mapping virtual URLs in the request to internal routes in the application.
Routes can be mapped to custom classes and callbacks, with optional configuration for tokens, patterns, HTTP methods, and more.

```hack
use Titon\Route\Route;
use Titon\Route\Router;
use Titon\Route\Group;

$router = Router::registry();
$router->map('login', new Route('/login', 'UsersController@login'));
$router->map('root', new Route('/', 'HomeController@index'));
```

Tokens can be placed within the virtual URL to extract values &mdash; `[]` captures numbers, `{}` captures strings,
`()` is a wildcard, and `<>` allows for custom regex patterns.

```hack
$router->map('blog.read', new Route('/blog/[year]/[month]/[day]/{slug}', 'Blog\ArticleController@read'));
```

Grouping routes and a filtering layer are also possible.

```hack
$router->filterCallback('auth', (Router $router, Route $route) ==> {
    if (!$auth->isAuthorized()) {
        throw new UnauthorizedException();
    }
});

$router->group((Router $router, Group $group) ==> {
    $group->addFilter('auth');

    $router->map(); // ...
});
```