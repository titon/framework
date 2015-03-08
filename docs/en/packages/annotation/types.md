# Type Aliases #

The following type aliases are available in the annotation package.

<table class="table is-striped">
    <thead>
        <tr>
            <th>Alias</th>
            <th>Type</th>
            <th>Description</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>Titon\Annotation\AnnotationMap</td>
            <td>Map&lt;string, Titon\Annotation\Annotation&gt;</td>
            <td>
                A mapping of <code>Titon\Annotation\Annotation</code> objects to their attribute name. 
                This map is primarily returned when reading annotations from a class or method.
            </td>
        </tr>
        <tr>
            <td>Titon\Annotation\ArgumentList</td>
            <td>array&lt;mixed&gt;</td>
            <td>A list of arguments passed to an annotation during instantiation.</td>
        </tr>
        <tr>
            <td>Titon\Annotation\MethodAnnotationMap</td>
            <td>Map&lt;string, Titon\Annotation\AnnotationMap&gt;</td>
            <td>
                A mapping of method names to <code>Titon\Annotation\AnnotationMap</code>s (previous). 
                This map is returned when reading annotations from all methods.
            </td>
        </tr>
        <tr>
            <td>Titon\Annotation\RegistryMap</td>
            <td>Map&lt;string, string&gt;</td>
            <td>
                A mapping of annotation attribute names to a fully qualified annotation classes.
            </td>
        </tr>
    </tbody>
</table>
