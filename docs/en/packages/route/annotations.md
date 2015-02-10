# Annotations #

The following annotations are available in the route package.

### Route(`string $key`, `string $path`[, `mixed $methods`[, `mixed $filters`[, `array<string, string> $patterns`]]]) ###

When declared, the `Route` annotation maps class and method annotations as routes, which in turn can be mapped with `Titon\Route\Router::wire()`. The annotations arguments define the unique route key, the custom URL path, the HTTP methods to match against, the filters to apply, and any patterns to tokenize. Any other route configurations should be handled outside of the annotation.

When an annotation is declared on a class, it will be treated as a resource using `Router::resource()`. All methods defined according to the resource map will be mapped as routes.

```hack
<<Route('users', '/user')>>
class UsersController extends RestController {
    public function index(): mixed {}
    public function create(): mixed {}
    public function read(int $id): mixed {}
    public function update(int $id): mixed {}
    public function delete(int $id): mixed {}
}
```

When an annotation is declared on a method, it will be mapped as a basic route using `Router::map()`.

```hack
class UsersController {
    <<Route('login', '/login')>>
    public function login(): mixed {}

    <<Route('login.post', '/login', 'POST')>>
    public function postLogin(): mixed {}

    <<Route('profile.edit', '/edit', [], ['auth'])>>
    public function editProfile(): mixed {}

    <<Route('profile', '/{username}', [], [], ['username' => '\w+'])>>
    public function viewProfile(string $username): mixed {}
}
```

Once annotations have been declared, we can map them using `Router::wire()`, which accepts the class name as a string.

```hack
$router->wire('UsersController');
```

For a better understanding of this annotation, here are a few declaration examples. The HTTP methods and filters arguments can either be strings, or an array of strings.

```hack
<<Route('forum.topic', '/forum/topic/[id]')>>
<<Route('blog', '/[year]/[month]/[day]/{slug}', [], [], ['slug' => '[\w\-]+'])>>
<<Route('account', '/account/settings', '', ['auth', 'csrf'])>>
<<Route('post.delete', '/posts/[id]', 'DELETE')>>
```
