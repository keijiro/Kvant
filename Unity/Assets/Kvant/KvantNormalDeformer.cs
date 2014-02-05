using UnityEngine;
using System.Collections;

public class KvantNormalDeformer : MonoBehaviour
{
    public bool smooth = false;
    public float noise = 0.5f;
    public float scale = 0.5f;
    public float speed = 0.5f;

    Mesh mesh;

    int[] sourceIndices;
    Vector3[] sourceVertices;
    Vector3[] sourceNormals;

    Vector3[] vertices;
    Vector3[] outVertices;

    float time;

    void Awake ()
    {
        var meshFilter = GetComponent<MeshFilter> ();
        var source = meshFilter.sharedMesh;

        sourceIndices = source.GetIndices (0);
        sourceVertices = source.vertices;
        sourceNormals = source.normals;

        mesh = new Mesh ();
        mesh.MarkDynamic ();

        if (smooth)
        {
            vertices = new Vector3 [sourceVertices.Length];

            mesh.vertices = vertices;
            mesh.uv = source.uv;
            mesh.uv2 = source.uv2;

            mesh.SetIndices (sourceIndices, MeshTopology.Triangles, 0);
        }
        else
        {
            vertices = new Vector3 [sourceVertices.Length];
            outVertices = new Vector3 [sourceIndices.Length];
            mesh.vertices = outVertices;

            var indices = new int [sourceIndices.Length];
            for (var i = 0; i < indices.Length; i++)
                indices [i] = i;

            var sourceUv = source.uv;
            if (sourceUv != null && sourceUv.Length > 0)
            {
                var uv = new Vector2 [sourceIndices.Length];
                for (var i = 0; i < indices.Length; i++)
                    uv[i] = sourceUv [sourceIndices [i]];
                mesh.uv = uv;
            }

            var sourceUv2 = source.uv2;
            if (sourceUv2 != null && sourceUv2.Length > 0)
            {
                var uv2 = new Vector2[sourceIndices.Length];
                for (var i = 0; i < indices.Length; i++)
                    uv2[i] = sourceUv2 [sourceIndices [i]];
                mesh.uv2 = uv2;
            }

            mesh.SetIndices (indices, MeshTopology.Triangles, 0);
        }

        meshFilter.sharedMesh = mesh;
    }

    void Update ()
    {
        time += speed * Time.deltaTime;

        var offs = Vector3.forward * time;

        for (var i = 0; i < vertices.Length; i++)
        {
            var sv = sourceVertices [i];
            var crd = (sv + offs) * scale;
            var disp = Kvant.Fractal4Coeffs(crd, 1, 2, 4, 8);
            vertices [i] = sv + sourceNormals [i] * disp * noise;
        }

        if (smooth)
        {
            mesh.vertices = vertices;
        }
        else
        {
            for (var i = 0; i < sourceIndices.Length; i++)
                outVertices[i] = vertices[sourceIndices[i]];
            
            mesh.vertices = outVertices;
        }

        mesh.RecalculateNormals ();
        mesh.RecalculateBounds ();
    }
}
